#pragma once

#include <node_object_wrap.h>
#include <cassert>
#include <thread>
#include <node.h>
#include <cmath>
#include <v8.h>

#include "async.hpp"

class backend : public node::ObjectWrap {
public:
    static void init(v8::Local<v8::Object> exports);
private:
    static v8::Persistent<v8::Function> constructor;
    static void alloc(const v8::FunctionCallbackInfo<v8::Value> & args);
    static void workload_test(const v8::FunctionCallbackInfo<v8::Value> & args);
};
