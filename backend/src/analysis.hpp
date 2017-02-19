#pragma once

#include <opencv2/imgproc/imgproc.hpp>

int find_background(cv::Mat & src, cv::Mat & mask);
int find_area(cv::Mat & src, cv::Mat & mask);
unsigned char find_max_height(cv::Mat & src, cv::Mat & mask, int bgHeight);
unsigned char find_min_height(cv::Mat & src, cv::Mat & mask, int bgHeight);
long find_volume(cv::Mat & src, cv::Mat & mask, int bgHeight);
long find_average_height(cv::Mat & src, cv::Mat & mask, int bgHeight);
void find_circularity(cv::Mat & mask);
