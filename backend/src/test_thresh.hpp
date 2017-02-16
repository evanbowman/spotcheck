#pragma once

#include <array>
#include <cstdint>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "make_cv_roi.hpp"

[[deprecated]] void test_thresh(const cv::Mat & src, const uint8_t thresh,
				const bool draw_circles,
				const std::array<int, 4> & backend_roi);
