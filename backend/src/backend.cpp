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

uv_mutex_t task_count_mtx;
static size_t task_count;

void backend::init(v8::Local<v8::Object> exports,
                   v8::Local<v8::Object> module) {
    using membr_type = void (*)(const callback_info &);
    static const std::array<std::pair<const char *, membr_type>, 7> mappings = {
        {{"import_source_image", import_source_image},
         {"import_source_gal", import_source_gal},
         {"launch_analysis", launch_analysis},
	 {"add_target", add_target},
	 {"test_thresh", test_thresh},
	 {"is_busy", is_busy},
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
    assert(uv_mutex_init(&::task_count_mtx) == 0);
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

inline static void populate_results_json(const std::vector<spot> & spots,
                                       std::ostream & ostr) {
    ostr << "[";
    const size_t index_max = spots.size() - 1;
    size_t index = 0;
    for (const auto & spot : spots) {
        spot.serialize(ostr);
        if (index != index_max) {
            ostr << ",";
        }
        index += 1;
    }
    ostr << "]" << std::endl;
}

void backend::test_thresh(const callback_info & args) {
 //    assert(args.Length() == 1);
//     auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
//     async::start(js_callback, [] {
// 	    auto outputTarget = m_source_image.clone();
// 	    cv::normalize(cv::InputArray(outputTarget),
// 			  cv::InputOutputArray(outputTarget),
// 			  0, 255, cv::NORM_MINMAX, CV_8UC1);
// 	    for (auto & target : m_targets) {
// 	        auto roi = make_cv_roi({{
// 			    target.fractStartx, target.fractStarty,
// 			    target.fractEndx, target.fractEndy
// 			}}, m_source_image);
// 		cv::Mat working_set = m_source_image(roi);
// 		// cv::normalize(cv::InputArray(working_set),
// 		// 	      cv::InputOutputArray(working_set),
// 		// 	      0, 255, cv::NORM_MINMAX, CV_8UC1);
// 		cv::threshold(working_set, working_set, target.threshold, 255, 3);
// 		working_set.copyTo(outputTarget(roi));
// 		const std::string output_dir = ::module_path +
// 		    "/../../../frontend/temp/tmp.png";
// 		std::cout << output_dir << std::endl;
// 		cv::imwrite(output_dir, outputTarget);
// 	    }
// 	    m_targets.clear();
// 	});
}

void backend::launch_analysis(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    std::cout << ::module_path << std::endl;
    for (auto & target : m_targets) {
	uv_mutex_lock(&::task_count_mtx);
	++::task_count;
	uv_mutex_unlock(&::task_count_mtx);
	async::start(js_callback, [target] {
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
		cv::Mat edges;
		cv::Canny(working_set, edges, 100, 200, 3);
		uv_mutex_lock(&::task_count_mtx);
		std::string fname = ::module_path + "/../../../frontend/temp/" +
		    std::to_string(target.rowId) +
		    std::to_string(target.colId) + ".png";
		std::cout << ::module_path << std::endl;
		cv::imwrite(fname, edges);
		--::task_count;
		uv_mutex_unlock(&::task_count_mtx);
	    });
    }
}

void backend::is_busy(const callback_info & args) {
    auto isolate = v8::Isolate::GetCurrent();
    args.GetReturnValue().Set(v8::Boolean::New(isolate, task_count > 0));
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

void backend::provide_norm_preview(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    async::start(js_callback, [&] {
	    preview_normalized(m_source_image);
	});
}
