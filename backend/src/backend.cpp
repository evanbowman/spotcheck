#include "backend.hpp"

v8::Persistent<v8::Function> backend::constructor;

cv::Mat backend::m_source_image;

void backend::init(v8::Local<v8::Object> exports) {
    using membr_type = void (*)(const callback_info &);
    static const std::array<std::pair<const char *, membr_type>, 2> mappings = {
        {{"import_source_image", import_source_image},
         {"import_source_gal", import_source_gal}}
    };
    static const char * js_class_name = "backend";
    v8::Isolate * isolate = exports->GetIsolate();
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
            // TODO: load gal and store in a member
        });
}
