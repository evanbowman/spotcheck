#include "backend.hpp"

v8::Persistent<v8::Function> backend::constructor;

cv::Mat backend::m_source_image;

uint8_t backend::m_threshold;

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

void backend::init(v8::Local<v8::Object> exports,
                   v8::Local<v8::Object> module) {
    using membr_type = void (*)(const callback_info &);
    static const std::array<std::pair<const char *, membr_type>, 4> mappings = {
        {{"import_source_image", import_source_image},
         {"import_source_gal", import_source_gal},
         {"launch_analysis", launch_analysis},
         {"set_threshold", set_threshold}}};
    static const char * js_class_name = "backend";
    v8::Isolate * isolate = exports->GetIsolate();
    ::module_path = get_mod_path(isolate, module);
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
        // std::fstream file(path);
        // auto ret = parse_gal(file);
        // if (ret) {
        //     m_source_gal = std::move(ret.unwrap());
        // }
    });
}

void backend::launch_analysis(const callback_info & args) {
    assert(args.Length() == 1);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    async::start(js_callback, [] { circ_score(m_source_image, m_threshold); });
}

void backend::set_threshold(const callback_info & args) {
    assert(args.Length() == 3);
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    m_threshold = args[1]->IntegerValue();
    bool draw_circles = args[2]->BooleanValue();
    async::start(js_callback, [draw_circles] {
        test_thresh(m_source_image, m_threshold, draw_circles);
    });
}
