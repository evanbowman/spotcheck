#pragma once

#include <node_object_wrap.h>
#include <cassert>
#include <node.h>
#include <cmath>
#include <v8.h>

#include "work_queue.hpp"

class backend : public node::ObjectWrap {
public:
    backend();
    static void init(v8::Local<v8::Object> exports);
private:
    work_queue m_workq;
    static void js_new(const v8::FunctionCallbackInfo<v8::Value> & args);
    static v8::Persistent<v8::Function> constructor;
};
