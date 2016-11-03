#include "backend.hpp"

#include <iostream>

v8::Persistent<v8::Function> backend::constructor;

static const unsigned int MIN_THREADS = 1;

backend::backend() :
    m_workq(std::max(::MIN_THREADS, std::thread::hardware_concurrency())) {
    m_workq.submit([]() {
            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(3));
                std::cout << "wassup" << std::endl;
            }
        });
}

void backend::init(v8::Local<v8::Object> exports) {
    v8::Isolate * isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate,
                                                                    js_new);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Backend"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // TODO: NODE_SET_PROTOTYPE_METHOD(tpl, "name", name);
    // : defines an interface for calling the class' methods from javascript
    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(v8::String::NewFromUtf8(isolate, "Backend"),
                 tpl->GetFunction());
}

void backend::js_new(const v8::FunctionCallbackInfo<v8::Value> & args) {
    assert(args.IsConstructCall());
    backend * obj = new backend();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
}
