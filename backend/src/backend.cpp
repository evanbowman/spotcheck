#include "backend.hpp"

v8::Persistent<v8::Function> backend::constructor;

void backend::init(v8::Local<v8::Object> exports) {
    v8::Isolate * isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate,
                                                                    alloc);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Backend"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "workloadTest", workload_test);
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

void backend::workload_test(const v8::FunctionCallbackInfo<v8::Value> & args) {
    auto js_callback = v8::Local<v8::Function>::Cast(args[0]);
    async::start(js_callback, [] {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        });
}
