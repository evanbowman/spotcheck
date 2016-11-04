#pragma once

#include <v8.h>
#include <uv.h>

using js_persist_cb = v8::Persistent<v8::Function>;
using js_cb = v8::Local<v8::Function>;

struct async {
    struct work {
        uv_work_t request;
        js_persist_cb callback;
        std::function<void()> work_routine;
    };
    template <typename F>
    static void start(js_cb & callback, F && work_routine) {
        auto isolate = v8::Isolate::GetCurrent();
        auto task = new work();
        task->work_routine = work_routine;
        task->request.data = task;
        task->callback.Reset(isolate, callback);
        uv_queue_work(uv_default_loop(), &task->request,
                      [](uv_work_t * req) {
                          work * task = static_cast<work *>(req->data);
                          task->work_routine();
                      },
                      [](uv_work_t * req, int status) {
                          v8::Isolate * isolate = v8::Isolate::GetCurrent();
                          static const unsigned argc = 0;
                          v8::Handle<v8::Value> * argv = nullptr;
                          work * task = static_cast<work *>(req->data);
                          v8::Local<v8::Function>::New(isolate, task->callback)->
                              Call(isolate->GetCurrentContext()->Global(),
                                   argc, argv);
                          task->callback.Reset();
                          delete task;
                      });
    }
};
