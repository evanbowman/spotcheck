#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdint>

void test_thresh(const cv::Mat & src, const uint8_t thresh,
                 const bool draw_circles);
