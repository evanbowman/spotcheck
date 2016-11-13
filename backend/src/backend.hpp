#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <node_object_wrap.h>
#include <cassert>
#include <string>
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
    static void import_source_image(const v8::FunctionCallbackInfo<v8::Value> & args);
    static void import_source_gal(const v8::FunctionCallbackInfo<v8::Value> & args);
    static cv::Mat m_source_image;
};
