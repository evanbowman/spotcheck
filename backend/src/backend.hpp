#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <node_object_wrap.h>
#include <cassert>
#include <utility>
#include <string>
#include <node.h>
#include <array>
#include <cmath>
#include <v8.h>

#include "async.hpp"

class backend : public node::ObjectWrap {
public:
    static void init(v8::Local<v8::Object> exports);
private:
    using callback_info = v8::FunctionCallbackInfo<v8::Value>;
    static v8::Persistent<v8::Function> constructor;
    static void alloc(const callback_info & args);
    static void import_source_image(const callback_info & args);
    static void import_source_gal(const callback_info & args);
    static cv::Mat m_source_image;
};
