#include "backend.hpp"

v8::Persistent<v8::Function> backend::constructor;

cv::Mat backend::m_source_image;

void backend::init(v8::Local<v8::Object> exports) {
    v8::Isolate * isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate,
                                                                    alloc);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Backend"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "import_source_image", import_source_image);
    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, "Backend"),
                 tpl->GetFunction());
}

void backend::alloc(const v8::FunctionCallbackInfo<v8::Value> & args) {
    assert(args.IsConstructCall());
    auto obj = new backend();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
}

void backend::import_source_image(const v8::FunctionCallbackInfo<v8::Value> & args) {
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    v8::String::Utf8Value str_arg(args[1]->ToString());
    std::string path(*str_arg);
    async::start(js_callback, [path] {
            m_source_image = cv::imread(path, 1);
        });
}

void backend::import_source_gal(const v8::FunctionCallbackInfo<v8::Value> & args) {
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    v8::String::Utf8Value str_arg(args[1]->ToString());
    std::string path(*str_arg);
    async::start(js_callback, [path] {
            // TODO: load gal and store in a member
        });
}
