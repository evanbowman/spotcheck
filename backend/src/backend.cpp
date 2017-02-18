#include "backend.hpp"

v8::Persistent<v8::Function> backend::constructor;

cv::Mat backend::m_source_image;

std::vector<backend::Target> backend::m_targets;

std::string module_path;

inline static std::string get_mod_path(v8::Isolate * isolate,
                                       v8::Local<v8::Object> & module) {
    auto require = v8::Local<v8::Function>::Cast(
        module->Get(v8::String::NewFromUtf8(isolate, "require")));
    auto filename = module->Get(v8::String::NewFromUtf8(isolate, "filename"))
                        .As<v8::String>();
    v8::Local<v8::Value> args[] = {v8::String::NewFromUtf8(isolate, "path")};
    auto path_module =
        require->Call(v8::Object::New(isolate), 1, args).As<v8::Object>();
    auto dirname = path_module->Get(v8::String::NewFromUtf8(isolate, "dirname"))
                       .As<v8::Function>();
    v8::Local<v8::Value> arg2[] = {filename};
    auto module_dirname = dirname->Call(path_module, 1, arg2).As<v8::String>();
    v8::String::Utf8Value utf8_mod_dirname(module_dirname);
    return *utf8_mod_dirname;
}

uv_mutex_t task_mtx;
static size_t task_count;

void backend::init(v8::Local<v8::Object> exports,
                   v8::Local<v8::Object> module) {
    using membr_type = void (*)(const callback_info &);
    static const std::array<std::pair<const char *, membr_type>, 10> mappings = {
        {{"import_source_image", import_source_image},
         {"import_source_gal", import_source_gal},
         {"split_sectors", split_sectors},
	 {"add_target", add_target},
	 {"clear_targets", clear_targets},
	 {"launch_analysis", launch_analysis},
	 {"update_target_thresh", update_target_thresh},
	 {"is_busy", is_busy},
	 {"get_target_thresh", get_target_thresh},
	 {"provide_norm_preview", provide_norm_preview}}};
    static const char * js_class_name = "backend";
    v8::Isolate * isolate = exports->GetIsolate();
    ::module_path = ::get_mod_path(isolate, module);
    v8::Local<v8::FunctionTemplate> tpl =
        v8::FunctionTemplate::New(isolate, alloc);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, js_class_name));
    tpl->InstanceTemplate()->SetInternalFieldCount(mappings.size());
    for (const auto & mapping : mappings) {
        NODE_SET_PROTOTYPE_METHOD(tpl, mapping.first, mapping.second);
    }
    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, js_class_name),
                 tpl->GetFunction());
    assert(uv_mutex_init(&::task_mtx) == 0);
}

void backend::alloc(const callback_info & args) {
    assert(args.IsConstructCall());
    auto obj = new backend();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
}

void backend::import_source_image(const callback_info & args) {
    assert(args.Length() == 2);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    v8::String::Utf8Value str_arg(args[1]->ToString());
    std::string path(*str_arg);
    async::start(js_callback, [path] {
        m_source_image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    });
}

void backend::import_source_gal(const callback_info & args) {
    assert(args.Length() == 2);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    v8::String::Utf8Value str_arg(args[1]->ToString());
    std::string path(*str_arg);
    async::start(js_callback, [path] {
	    // TODO: load source gal...
	});
}

// inline static void populate_results_json(const std::vector<spot> & spots,
//                                        std::ostream & ostr) {
//     ostr << "[";
//     const size_t index_max = spots.size() - 1;
//     size_t index = 0;
//     for (const auto & spot : spots) {
//         spot.serialize(ostr);
//         if (index != index_max) {
//             ostr << ",";
//         }
//         index += 1;
//     }
//     ostr << "]" << std::endl;
// }

static int consume_connected(cv::Mat & src, int x, int y) {
    int count = 0;
    using coord = std::pair<int, int>;
    std::stack<coord> stack;
    stack.push({x, y});
    const auto action = [&src, &stack, &count](coord & c, int xOff,
					       int yOff) {
			    const int i = c.first + xOff;
			    const int j = c.second + yOff;
			    if (i > 0 && i < src.rows - 1 && j > 0 && j < src.cols - 1) {
				if (src.at<unsigned char>(i, j) != 0) {
				    src.at<unsigned char>(i, j) = 0;
				    ++count;
				    stack.push({i, j});
				}
			    }
			};
    action(stack.top(), 0, 0);
    while (!stack.empty()) {
        coord coord = stack.top();
        stack.pop();
        action(coord, -1, 0);
        action(coord, 0, 1);
        action(coord, 0, -1);
        action(coord, 1, 0);
    }
    return count;
}

static void mask_largest_connected(cv::Mat & src) {
    assert(src.depth() != sizeof(uchar));
    assert(src.channels() == 1);
    using region_info = std::tuple<int, int, int>;
    std::vector<region_info> regions;
    auto src_cpy = src.clone();
    for (int i = 0; i < src_cpy.rows; ++i) {
	for (int j = 0; j < src_cpy.cols; ++j) {
	    auto pixel = src_cpy.at<unsigned char>(i, j);
	    if (pixel != 0) {
		regions.emplace_back(consume_connected(src_cpy, i, j), i, j);
	    }
	}
    }
    if (!regions.empty()) {
	auto max = std::max_element(regions.begin(), regions.end(),
				    [](const region_info & lhs, const region_info & rhs) {
					return std::get<0>(lhs) < std::get<0>(rhs);
				    });
	regions.erase(max);
	for (auto & region : regions) {
	    consume_connected(src, std::get<1>(region), std::get<2>(region));
	}
    }
    cv::threshold(src, src, 127, 255, cv::THRESH_BINARY);
}

static void process_sector(const backend::Target & target, const cv::Mat & src) {
    auto roi = make_cv_roi({{
		target.fractStartx, target.fractStarty,
		target.fractEndx, target.fractEndy
	    }}, src);
    cv::Mat working_set = src(roi);
    cv::cvtColor(working_set, working_set, CV_BGR2GRAY);
    cv::Mat norm, thresh;
    cv::normalize(cv::InputArray(working_set),
		  cv::InputOutputArray(norm),
		  0, 255, cv::NORM_MINMAX, CV_8UC1);
    cv::threshold(norm, thresh, target.threshold, 255, 3);
    mask_largest_connected(thresh);
    uv_mutex_lock(&::task_mtx);
    auto suffix = std::to_string(target.rowId) + std::to_string(target.colId);
    static const auto extension = ".png";
    const std::string origin_fname = ::module_path
	+ "/../../../frontend/temp/original" + suffix + extension;
    cv::imwrite(origin_fname, working_set);
    const std::string segment_fname = ::module_path
	+ "/../../../frontend/temp/mask" + suffix + extension;
    cv::imwrite(segment_fname, thresh);
    const std::string norm_fname = ::module_path
	+ "/../../../frontend/temp/norm" + suffix + extension;
    cv::imwrite(norm_fname, norm);
    uv_mutex_unlock(&::task_mtx);
}

void backend::split_sectors(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    for (auto & target : m_targets) {
	uv_mutex_lock(&::task_mtx);
	++::task_count;
	uv_mutex_unlock(&::task_mtx);
	async::start(js_callback, [&target] {
		auto roi = make_cv_roi({{
			    target.fractStartx, target.fractStarty,
			    target.fractEndx, target.fractEndy
			}}, m_source_image);
	        cv::Mat working_set = m_source_image(roi);
		cv::cvtColor(working_set, working_set, CV_BGR2GRAY);
		cv::normalize(cv::InputArray(working_set),
			      cv::InputOutputArray(working_set),
			      0, 255, cv::NORM_MINMAX, CV_8UC1);
		cv::Scalar avg_intensity = cv::mean(working_set);
		cv::threshold(working_set, working_set, avg_intensity[0], 255, 3);
	        target.threshold = avg_intensity[0];
		process_sector(target, m_source_image);
		uv_mutex_lock(&::task_mtx);
		--::task_count;
		uv_mutex_unlock(&::task_mtx);
	    });
    }
}

void backend::is_busy(const callback_info & args) {
    auto isolate = v8::Isolate::GetCurrent();
    args.GetReturnValue().Set(v8::Boolean::New(isolate, task_count > 0));
}

int find_background(cv::Mat & src, cv::Mat & mask) {
  long sum = 0;
  int  quant = 0;
  for(int i=0; i < mask.rows ; ++i){
    for( int j = 0; j < mask.cols; ++j){
      if(mask.at<unsigned char>(i,j)==0){
          sum += src.at<unsigned char>(i,j);
          quant++;
      }
    }
  }
  return sum / quant;
}


long find_volume(cv::Mat & src, cv::Mat & mask, int bgHeight){
  long volume = 0;

  for(int i=0; i < mask.rows ; ++i){
    for(int j = 0; j < mask.cols; ++j){
      if(mask.at<unsigned char>(i,j)>0){
          volume += src.at<unsigned char>(i,j) - bgHeight;
      }
    }
  }

  return volume;
}

void backend::analyze_target(Target & target, cv::Mat & src, cv::Mat & mask) {
    // ...
    // Find Background
    int background_avg_height = find_background(src, mask);

    // Background Subtraction
    long volume = find_volume(src,mask, background_avg_height);

    std::cout << background_avg_height << " " << volume << std::endl;

}


void backend::launch_analysis(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    for (auto & target : m_targets) {
	uv_mutex_lock(&::task_mtx);
	++::task_count;
	uv_mutex_unlock(&::task_mtx);
	async::start(js_callback, [&target] {
		cv::Mat src, mask;
		static const auto extension = ".png";
		const std::string original_loc = "/../../../frontend/temp/original";
		const std::string mask_loc = "/../../../frontend/temp/mask";
		const auto suffix = std::to_string(target.rowId) +
		    std::to_string(target.colId);
		uv_mutex_lock(&::task_mtx);
		src = cv::imread(::module_path + original_loc + suffix + extension,
				 CV_LOAD_IMAGE_GRAYSCALE);
		mask = cv::imread(::module_path + mask_loc + suffix + extension,
				  CV_LOAD_IMAGE_GRAYSCALE);
		assert(!src.empty());
		assert(!mask.empty());
		uv_mutex_unlock(&::task_mtx);
		analyze_target(target, src, mask);
		uv_mutex_lock(&::task_mtx);
		--::task_count;
		uv_mutex_unlock(&::task_mtx);
	    });
    }
}

void backend::update_target_thresh(const callback_info & args) {
    assert(args.Length() == 4);
    const int64_t targetRow = args[1]->IntegerValue();
    const int64_t targetCol = args[2]->IntegerValue();
    auto target = std::find_if(m_targets.begin(), m_targets.end(),
			       [targetRow, targetCol](const Target & target) {
				   return target.rowId == targetRow && target.colId == targetCol;
			       });
    assert(target != m_targets.end());
    target->threshold = args[3]->IntegerValue();
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    async::start(js_callback, [target] {
	    process_sector(*target, m_source_image);
	});
}

void backend::add_target(const callback_info & args) {
    assert(args.Length() == 7);
    m_targets.push_back({
	    v8::Local<v8::Integer>::Cast(args[0])->Value(),
	    v8::Local<v8::Integer>::Cast(args[1])->Value(),
	    std::min(1.0, v8::Local<v8::Number>::Cast(args[2])->Value()),
	    std::min(1.0, v8::Local<v8::Number>::Cast(args[3])->Value()),
	    std::min(1.0, v8::Local<v8::Number>::Cast(args[4])->Value()),
	    std::min(1.0, v8::Local<v8::Number>::Cast(args[5])->Value()),
	    args[6]->IntegerValue()
	});
}

void backend::clear_targets(const callback_info & args) {
    assert(args.Length() == 0);
    m_targets.clear();
}

void backend::provide_norm_preview(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    async::start(js_callback, [&] {
	    preview_normalized(m_source_image);
	});
}

void backend::get_target_thresh(const callback_info & args) {
    assert(args.Length() == 2);
    const int64_t targetRow = args[0]->IntegerValue();
    const int64_t targetCol = args[1]->IntegerValue();
    auto target = std::find_if(m_targets.begin(), m_targets.end(),
			       [targetRow, targetCol](const Target & target) {
				   return target.rowId == targetRow && target.colId == targetCol;
			       });
    assert(target != m_targets.end());
    auto isolate = v8::Isolate::GetCurrent();
    args.GetReturnValue().Set(v8::Integer::New(isolate, target->threshold));
}
