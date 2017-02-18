#pragma once

#include "make_cv_roi.hpp"
#include "async.hpp"
#include "preview_normalized.hpp"
#include "results.hpp"

#include <iostream>
#include <array>
#include <random>
#include <cassert>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <node.h>
#include <node_object_wrap.h>
#include <opencv2/highgui/highgui.hpp>
#include <stack>
#include <tuple>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <utility>
#include <v8.h>

class backend : public node::ObjectWrap {
public:
    static void init(v8::Local<v8::Object> exports,
                     v8::Local<v8::Object> module);
    struct Target {
	int64_t rowId, colId;
	double fractStartx, fractStarty;
        double fractEndx, fractEndy;
        int64_t threshold;
    };
private:
    using callback_info = v8::FunctionCallbackInfo<v8::Value>;
    static v8::Persistent<v8::Function> constructor;
    static void alloc(const callback_info & args);
    static void import_source_image(const callback_info & args);
    static void import_source_gal(const callback_info & args);
    static void split_sectors(const callback_info & args);
    static void clear_targets(const callback_info & args);
    static void launch_analysis(const callback_info & args);
    static void analyze_target(Target & target, cv::Mat & src, cv::Mat & mask);
    static void add_target(const callback_info & args);
    static void get_target_thresh(const callback_info & args);
    static void write_results_JSON(const callback_info & args);
    static void update_target_thresh(const callback_info & args);
    static void provide_norm_preview(const callback_info & args);
    static void is_busy(const callback_info & args);
    static std::vector<result> m_results;
    static cv::Mat m_source_image;
    static std::vector<Target> m_targets;
};
