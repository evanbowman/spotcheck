#include "backend.hpp"
#include "analysis.hpp"

v8::Persistent<v8::Function> Backend::constructor;

std::map<std::pair<int64_t, int64_t>, Result> Backend::m_results;

cv::Mat Backend::m_source_image;

std::vector<Backend::Target> Backend::m_targets;

std::map<std::string, std::string> Backend::m_usr_scripts;

std::string module_path;

double Backend::m_pixel_width = 1.0;

static const std::array<std::string, 1> g_builtins{{"circularity"}};

std::set<std::string> Backend::m_enabled_builtins;

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

void Backend::init(v8::Local<v8::Object> exports,
                   v8::Local<v8::Object> module) {
    using membr_type = void (*)(const callback_info &);
    static const std::array<std::pair<const char *, membr_type>, 13> mappings =
        {{{"import_source_image", import_source_image},
          {"split_sectors", split_sectors},
          {"add_target", add_target},
          {"clear_targets", clear_targets},
          {"launch_analysis", launch_analysis},
          {"update_target_thresh", update_target_thresh},
          {"is_busy", is_busy},
          {"write_results_JSON", write_results_JSON},
          {"get_target_thresh", get_target_thresh},
          {"provide_norm_preview", provide_norm_preview},
          {"configure", configure},
	  {"set_pixel_width", set_pixel_width},
          {"write_default_config", write_default_config}}};
    static const char * js_class_name = "Backend";
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

void Backend::set_pixel_width(const callback_info & args) {
    assert(args.Length() == 1);
    auto num = v8::Local<v8::Number>::Cast(args[0]);
    m_pixel_width = num->Value();
}

void Backend::alloc(const callback_info & args) {
    assert(args.IsConstructCall());
    auto obj = new Backend();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
}

void Backend::import_source_image(const callback_info & args) {
    assert(args.Length() == 2);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    v8::String::Utf8Value str_arg(args[1]->ToString());
    std::string path(*str_arg);
    async::start(js_callback, [path] {
        m_source_image = cv::imread(path, CV_LOAD_IMAGE_COLOR);
    });
}

static int consume_connected(cv::Mat & src, int x, int y) {
    int count = 0;
    using coord = std::pair<int, int>;
    std::stack<coord> stack;
    stack.push({x, y});
    const auto action = [&src, &stack, &count](coord & c, int xOff, int yOff) {
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
        auto max = std::max_element(
            regions.begin(), regions.end(),
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

static void process_sector(const Backend::Target & target,
                           const cv::Mat & src) {
    auto roi = make_cv_roi({{target.fractStartx, target.fractStarty,
                             target.fractEndx, target.fractEndy}},
                           src);
    cv::Mat working_set = src(roi);
    cv::cvtColor(working_set, working_set, CV_BGR2GRAY);
    cv::Mat norm, thresh;
    cv::normalize(cv::InputArray(working_set), cv::InputOutputArray(norm), 0,
                  255, cv::NORM_MINMAX, CV_8UC1);
    cv::threshold(norm, thresh, target.threshold, 255, 3);
    mask_largest_connected(thresh);
    uv_mutex_lock(&::task_mtx);
    auto suffix = std::to_string(target.rowId) + std::to_string(target.colId);
    static const auto extension = ".png";
    const std::string origin_fname =
        ::module_path + "/../../../frontend/temp/original" + suffix + extension;
    cv::imwrite(origin_fname, working_set);
    const std::string segment_fname =
        ::module_path + "/../../../frontend/temp/mask" + suffix + extension;
    cv::imwrite(segment_fname, thresh);
    const std::string norm_fname =
        ::module_path + "/../../../frontend/temp/norm" + suffix + extension;
    cv::imwrite(norm_fname, norm);
    uv_mutex_unlock(&::task_mtx);
}

void Backend::split_sectors(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    uv_mutex_lock(&::task_mtx);
    ::task_count = m_targets.size();
    uv_mutex_unlock(&::task_mtx);
    for (auto & target : m_targets) {
        async::start(js_callback, [&target] {
            auto roi = make_cv_roi({{target.fractStartx, target.fractStarty,
                                     target.fractEndx, target.fractEndy}},
                                   m_source_image);
            cv::Mat working_set = m_source_image(roi);
            cv::cvtColor(working_set, working_set, CV_BGR2GRAY);
            cv::normalize(cv::InputArray(working_set),
                          cv::InputOutputArray(working_set), 0, 255,
                          cv::NORM_MINMAX, CV_8UC1);
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

void Backend::is_busy(const callback_info & args) {
    auto isolate = v8::Isolate::GetCurrent();
    args.GetReturnValue().Set(v8::Boolean::New(isolate, task_count > 0));
}

static inline void populate_results_JSON(
    const std::map<std::pair<int64_t, int64_t>, Result> & results,
    std::ostream & ostr) {
    ostr << "[";
    const size_t max_index = results.size() - 1;
    size_t index = 0;
    for (auto & res : results) {
        res.second.serialize(ostr);
        if (index != max_index) {
            ostr << ",";
        }
        ++index;
    }
    ostr << "]" << std::endl;
}

void Backend::write_results_JSON(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    async::start(js_callback, [] {
        std::fstream file(::module_path +
                              "/../../../frontend/temp/results.json",
                          std::fstream::out);
        populate_results_JSON(m_results, file);
    });
}

void Backend::analyze_target(Target & target, cv::Mat & src, cv::Mat & mask) {
    auto result = m_results.find({target.rowId, target.colId});
    const int background_avg_height = find_background(src, mask);
    const double volume = find_volume(src, mask, background_avg_height);
    const double circularity = (volume > 0) ? find_circularity(mask) : 0.0;
    if (m_enabled_builtins.find("circularity") != m_enabled_builtins.end()) {
        uv_mutex_lock(&task_mtx);
        result->second.add_data({"circularity", circularity});
        uv_mutex_unlock(&task_mtx);
    }
}

static cv::Mat load_src_img(const Backend::Target & target) {
    static const auto extension = ".png";
    const std::string original_loc = "/../../../frontend/temp/original";
    const auto suffix =
        std::to_string(target.rowId) + std::to_string(target.colId);
    uv_mutex_lock(&::task_mtx);
    cv::Mat src = cv::imread(::module_path + original_loc + suffix + extension,
                             CV_LOAD_IMAGE_GRAYSCALE);
    uv_mutex_unlock(&::task_mtx);
    return src;
}

static cv::Mat load_mask_img(const Backend::Target & target) {
    static const auto extension = ".png";
    const std::string mask_loc = "/../../../frontend/temp/mask";
    const auto suffix =
        std::to_string(target.rowId) + std::to_string(target.colId);
    uv_mutex_lock(&::task_mtx);
    cv::Mat mask = cv::imread(::module_path + mask_loc + suffix + extension,
                              CV_LOAD_IMAGE_GRAYSCALE);
    uv_mutex_unlock(&::task_mtx);
    return mask;
}

static v8::Handle<v8::Object>
v8_wrap_cv_mat(v8::Isolate * isolate, cv::Mat & mat,
               v8::Handle<v8::FunctionTemplate> & tmpl) {
    v8::Handle<v8::Object> obj = tmpl->GetFunction()->NewInstance();
    obj->SetInternalField(0, v8::External::New(isolate, &mat));
    obj->Set(v8::String::NewFromUtf8(isolate, "rows"),
             v8::Number::New(isolate, mat.rows));
    obj->Set(v8::String::NewFromUtf8(isolate, "cols"),
             v8::Number::New(isolate, mat.cols));
    obj->Set(
        v8::String::NewFromUtf8(isolate, "at"),
        v8::Function::New(isolate, [](const Backend::callback_info & args) {
            v8::Local<v8::Object> self = args.Holder();
            auto wrap =
                v8::Local<v8::External>::Cast(self->GetInternalField(0));
            auto mat_ptr = static_cast<cv::Mat *>(wrap->Value());
            const int row = v8::Local<v8::Number>::Cast(args[0])->Value();
            const int col = v8::Local<v8::Number>::Cast(args[1])->Value();
            if (row >= mat_ptr->rows || col > mat_ptr->cols || row < 0 ||
                col < 0) {
                args.GetReturnValue().Set(v8::Null(args.GetIsolate()));
            } else {
                args.GetReturnValue().Set(v8::Integer::New(
                    args.GetIsolate(), mat_ptr->at<unsigned char>(row, col)));
            }

        }));
    return obj;
}

static void build_img_caches(std::map<std::pair<int64_t, int64_t>, cv::Mat> & src_cache,
			     std::map<std::pair<int64_t, int64_t>, cv::Mat> & mask_cache,
			     const std::vector<Backend::Target> & targets) {
    for (auto & target : targets) {
	if (src_cache.find({target.rowId, target.colId}) == src_cache.end()) {
	    src_cache[{target.rowId, target.colId}] = load_src_img(target);
	}
	if (mask_cache.find({target.rowId, target.colId}) == mask_cache.end()) {
	    mask_cache[{target.rowId, target.colId}] = load_mask_img(target);
	}
    }
}

void Backend::run_user_metrics() {
    v8::Isolate * isolate = v8::Isolate::GetCurrent();
    v8::HandleScope handle_scope(isolate);
    auto context = v8::Context::New(isolate);
    context->AllowCodeGenerationFromStrings(false);
    context->Enter();
    v8::Handle<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New(isolate);
    v8::Local<v8::ObjectTemplate> instance_t = tmpl->InstanceTemplate();
    instance_t->SetInternalFieldCount(1);
    std::map<std::pair<int64_t, int64_t>, cv::Mat> src_cache;
    std::map<std::pair<int64_t, int64_t>, cv::Mat> mask_cache;
    build_img_caches(src_cache, mask_cache, m_targets);
    for (const auto & scr_node : m_usr_scripts) {
        v8::Handle<v8::String> code =
            v8::String::NewFromUtf8(isolate, scr_node.second.c_str());
        auto script = v8::Script::Compile(code);
        script->Run();
        auto entry = isolate->GetCurrentContext()->Global()->Get(
            v8::String::NewFromUtf8(isolate, "main"));
        auto fn = v8::Local<v8::Function>::New(
            isolate, v8::Handle<v8::Function>::Cast(entry));
        for (const auto & target : m_targets) {
	    std::array<v8::Handle<v8::Value>, 2> argv{
                {v8_wrap_cv_mat(isolate, src_cache[{target.rowId, target.colId}], tmpl),
                 v8_wrap_cv_mat(isolate, mask_cache[{target.rowId, target.colId}], tmpl)}};
            float result = v8::Local<v8::Number>::Cast(
                               fn->Call(isolate->GetCurrentContext()->Global(),
                                        argv.size(), argv.data()))
                               ->Value();
            m_results[{target.rowId, target.colId}].add_data(
                {scr_node.first, result});
        }
    }
    context->Exit();
}

void Backend::launch_analysis(const callback_info & args) {
    assert(args.Length() == 1);
    m_results.clear();
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    uv_mutex_lock(&::task_mtx);
    ::task_count = m_targets.size();
    uv_mutex_unlock(&::task_mtx);
    for (const auto & target : m_targets) {
        Result result;
        result.add_data({"row", target.rowId});
        result.add_data({"col", target.colId});
        m_results[{target.rowId, target.colId}] = result;
    }
    run_user_metrics();
    for (auto & target : m_targets) {
        async::start(js_callback, [&target] {
            auto src = load_src_img(target);
            auto mask = load_mask_img(target);
            analyze_target(target, src, mask);
            uv_mutex_lock(&::task_mtx);
            --::task_count;
            uv_mutex_unlock(&::task_mtx);
        });
    }
}

void Backend::update_target_thresh(const callback_info & args) {
    assert(args.Length() == 4);
    const int64_t targetRow = args[1]->IntegerValue();
    const int64_t targetCol = args[2]->IntegerValue();
    auto target = std::find_if(m_targets.begin(), m_targets.end(),
                               [targetRow, targetCol](const Target & target) {
                                   return target.rowId == targetRow &&
                                          target.colId == targetCol;
                               });
    assert(target != m_targets.end());
    target->threshold = args[3]->IntegerValue();
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    async::start(js_callback,
                 [target] { process_sector(*target, m_source_image); });
}

void Backend::add_target(const callback_info & args) {
    assert(args.Length() == 7);
    m_targets.push_back(
        {v8::Local<v8::Integer>::Cast(args[0])->Value(),
         v8::Local<v8::Integer>::Cast(args[1])->Value(),
         std::min(1.0, v8::Local<v8::Number>::Cast(args[2])->Value()),
         std::min(1.0, v8::Local<v8::Number>::Cast(args[3])->Value()),
         std::min(1.0, v8::Local<v8::Number>::Cast(args[4])->Value()),
         std::min(1.0, v8::Local<v8::Number>::Cast(args[5])->Value()),
         args[6]->IntegerValue()});
}

static void add_stdlib_to_default_config(std::string & buffer) {
    static const std::string area_metric =
        "\\n"
        "function main(src, mask) {\\n"
        "  var area = 0;\\n"
        "  for (var i = 0; i < mask.rows; ++i) {\\n"
        "    for (var j = 0; j < mask.cols; ++j) {\\n"
        "      if (mask.at(i, j) !== 0) {\\n"
        "        area += 1;\\n"
        "      }\\n"
        "    }\\n"
        "  }\\n"
        "  return area;\\n"
        "}";
    buffer += "\"area\":{\"builtin\":false,\"enabled\":true,\"src\":\"" +
              area_metric + "\"},";
    static const std::string volume_metric =
        "\\n"
        "function main(src, mask) {\\n"
        "  var avgBkg = findAvgBackground(src, mask);\\n"
        "  var volume = 0;\\n"
        "  for (var i = 0; i < src.rows; ++i) {\\n"
        "    for (var j = 0; j < src.cols; ++j) {\\n"
        "      if (mask.at(i, j) > 0) {\\n"
        "        volume += src.at(i, j) - avgBkg;\\n"
        "      }\\n"
        "    }\\n"
        "  }\\n"
        "  return volume;\\n"
        "}\\n"
        "\\n"
        "function findAvgBackground(src, mask) {\\n"
        "  var sum = 0;\\n"
        "  var quant = 0;\\n"
        "  for (var i = 0; i < mask.rows; ++i) {\\n"
        "    for (var j = 0; j < mask.cols; ++j) {\\n"
        "      if (mask.at(i, j) === 0) {\\n"
        "        sum += src.at(i, j);\\n"
        "        quant++;\\n"
        "      }\\n"
        "    }\\n"
        "  }\\n"
        "  return sum / Math.max(quant, 1);\\n"
        "}\\n";
    buffer += "\"volume\":{\"builtin\":false,\"enabled\":true,\"src\":\"" +
              volume_metric + "\"},";
    static const std::string radius_metric =
	"\\n"
	"function main(src, mask) {\\n"
	"  var centroid = findCentroid(mask);\\n"
	"  var edges = findEdges(mask);\\n"
	"  var radius = 0;\\n"
	"  for (var i = 0; i < edges.length; ++i) {\\n"
	"    radius += Math.sqrt(Math.pow(centroid.x - edges[i].x, 2) +\\n"
	"                        Math.pow(centroid.y - edges[i].y, 2));\\n"
	"  }\\n"
	"  return radius / Math.max(1, edges.length);\\n"
	"}\\n"
	"\\n"
	"function findEdges(mask) {\\n"
	"  edges = [];\\n"
	"  for (var i = 0; i < mask.rows; ++i) {\\n"
	"    for (var j = 0; j < mask.cols; ++j) {\\n"
	"      if (mask.at(i, j) > 0) {\\n"
	"        if (mask.at(i - 1, j) === 0 ||\\n"
	"            mask.at(i + 1, j) === 0 ||\\n"
	"            mask.at(i, j - 1) === 0 ||\\n"
	"            mask.at(i, j + 1) === 0) {\\n"
	"          edges.push({\'x\': i, \'y\': j});\\n"
	"        }\\n"
	"      }\\n"
	"    }\\n"
	"  }\\n"
	"  return edges;\\n"
	"}\\n"
	"\\n"
	"function findCentroid(mask) {\\n"
	"  var xSum = 0, ySum = 0, area = 0;\\n"
	"  for (var i = 0; i < mask.rows; ++i) {\\n"
	"    for (var j = 0; j < mask.cols; ++j) {\\n"
	"      if (mask.at(i, j) !== 0) {\\n"
	"        xSum += i;\\n"
	"        ySum += j;\\n"
	"        area += 1;\\n"
	"      }\\n"
	"    }\\n"
	"  }\\n"
	"  return {\\n"
	"    \'x\': xSum / area,\\n"
	"    \'y\': ySum / area\\n"
	"  };\\n"
	"}\\n";
    buffer += "\"average radius\":{\"builtin\":false,\"enabled\":true,\"src\":\"" +
              radius_metric + "\"},";
}

void Backend::write_default_config(const callback_info & args) {
    assert(args.Length() == 1);
    std::string out = "{\"metrics\":{";
    for (const auto & builtin : g_builtins) {
        out += "\"" + builtin + "\":{\"builtin\":true,\"enabled\":true},";
    }
    add_stdlib_to_default_config(out);
    out.pop_back();
    out += "}}";
    v8::String::Utf8Value str_arg(args[0]->ToString());
    std::string path(*str_arg);
    std::fstream out_file(path, std::fstream::out);
    out_file << out;
}

void Backend::configure(const callback_info & args) {
    assert(args.Length() == 1);
    v8::String::Utf8Value str_arg(args[0]->ToString());
    std::string path(*str_arg);
    std::fstream config_file(path);
    std::stringstream ss;
    ss << config_file.rdbuf();
    rapidjson::Document d;
    d.Parse(ss.str().c_str());
    m_enabled_builtins.clear();
    m_usr_scripts.clear();
    auto & metrics = d["metrics"];
    for (auto it = metrics.MemberBegin(); it != metrics.MemberEnd(); ++it) {
        if (it->value["enabled"].GetBool()) {
            if (it->value["builtin"].GetBool()) {
                m_enabled_builtins.insert(it->name.GetString());
            } else {
                m_usr_scripts[it->name.GetString()] = it->value["src"].GetString();
            }
        }
    }
}

void Backend::clear_targets(const callback_info & args) {
    assert(args.Length() == 0);
    m_targets.clear();
}

void Backend::provide_norm_preview(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    async::start(js_callback, [&] { preview_normalized(m_source_image); });
}

void Backend::get_target_thresh(const callback_info & args) {
    assert(args.Length() == 2);
    const int64_t targetRow = args[0]->IntegerValue();
    const int64_t targetCol = args[1]->IntegerValue();
    auto target = std::find_if(m_targets.begin(), m_targets.end(),
                               [targetRow, targetCol](const Target & target) {
                                   return target.rowId == targetRow &&
                                          target.colId == targetCol;
                               });
    assert(target != m_targets.end());
    auto isolate = v8::Isolate::GetCurrent();
    args.GetReturnValue().Set(v8::Integer::New(isolate, target->threshold));
}
