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
#include "spot.hpp"

std::vector<spot> find_spots(const cv::Mat & src, const uint8_t thresh,
                             std::array<int, 4> & backend_roi);

void circ_score(std::vector<spot> & spots);
