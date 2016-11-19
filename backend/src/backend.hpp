#pragma once

#include "async.hpp"
#include "circ_score.hpp"
#include "parse_gal.hpp"
#include "test_thresh.hpp"

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
    static void init(v8::Local<v8::Object> exports,
                     v8::Local<v8::Object> module);

private:
    using callback_info = v8::FunctionCallbackInfo<v8::Value>;
    static v8::Persistent<v8::Function> constructor;
    static void alloc(const callback_info & args);
    static void import_source_image(const callback_info & args);
    static void import_source_gal(const callback_info & args);
    static void launch_analysis(const callback_info & args);
    static void set_threshold(const callback_info & args);
    static void set_roi(const callback_info & args);
    static cv::Mat m_source_image;
    static uint8_t m_threshold;
    static std::array<int, 4> m_roi;
};
