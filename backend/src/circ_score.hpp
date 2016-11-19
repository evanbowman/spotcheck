#pragma once

#include <cstdint>
#include <iostream>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "make_cv_roi.hpp"

std::vector<double> circ_score(const cv::Mat & source_img, const uint8_t thresh,
                               const std::array<int, 4> & backend_roi);
