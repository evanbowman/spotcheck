#include "backend.hpp"

v8::Persistent<v8::Function> backend::constructor;

void backend::init(v8::Local<v8::Object> exports) {
    v8::Isolate * isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate,
                                                                    js_new);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Backend"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // TODO: NODE_SET_PROTOTYPE_METHOD(tpl, "name", name);
    NODE_SET_PROTOTYPE_METHOD(tpl, "workloadTest", workload_test);
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

struct work {
    uv_work_t request;
    v8::Persistent<v8::Function> callback;
};

void backend::workload_test(const v8::FunctionCallbackInfo<v8::Value> & args) {
    v8::Isolate * isolate = args.GetIsolate();
    work * wk = new work();
    wk->request.data = wk;
    auto callback = v8::Local<v8::Function>::Cast(args[0]);
    wk->callback.Reset(isolate, callback);
    uv_queue_work(uv_default_loop(),
                  &wk->request,
                  [](uv_work_t * req) {
                      std::this_thread::sleep_for(std::chrono::seconds(3));
                  },
                  [](uv_work_t * req, int status) {
                      v8::Isolate * isolate = v8::Isolate::GetCurrent();
                      // v8::HandleScope(isolate);
                      static const unsigned argc = 0;
                      v8::Handle<v8::Value> * argv = nullptr;
                      work * wk = static_cast<work *>(req->data);
                      v8::Local<v8::Function>::New(isolate, wk->callback)->
                          Call(isolate->GetCurrentContext()->Global(), argc, argv);
                      wk->callback.Reset();
                      delete wk;
                  });
}
