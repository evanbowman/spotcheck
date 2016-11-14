#pragma once

#include "async.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <node.h>
#include <node_object_wrap.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <utility>
#include <v8.h>

class backend : public node::ObjectWrap {
public:
    static void init(v8::Local<v8::Object> exports);

private:
    using callback_info = v8::FunctionCallbackInfo<v8::Value>;
    static v8::Persistent<v8::Function> constructor;
    static void alloc(const callback_info & args);
    static void import_source_image(const callback_info & args);
    static void import_source_gal(const callback_info & args);
    static void launch_analysis(const callback_info & args);
    static cv::Mat m_source_image;
};
