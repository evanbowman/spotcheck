#pragma once

#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

//! @brief Calculates a target's the average background value
//!
//! This function calculates the average background value for a
//! subimage. It does so by computing an average of all of the src
//! pixels that correspond to a binary zero mask value.
//!
//! @param src The heightmap source subimage
//! @param mask The binary droplet detection mask
int find_background(cv::Mat & src, cv::Mat & mask);

//! @brief Calculates a target's area
//!
//! This function calulates a target's area by computing an integral
//! of all of the binary mask's nonzero pixels.
//!
//! @param src The heightmap source subimage
//! @param mask The binary droplet dection mask
int find_area(cv::Mat & src, cv::Mat & mask);

//! @breif Calculates a target's max height
//!
//! This function does a simple search for a droplet's max height, by
//! looking through all the pixels that correspond to a nonzero binary
//! mask value and picking the largest one.
//!
//! @param src The heightmap source subimage
//! @param mask The binary droplet dection mask
unsigned char find_max_height(cv::Mat & src, cv::Mat & mask, int bgHeight);

//! @breif Calculates a target's max height
//!
//! This function does a simple search for a droplet's max height, by
//! looking through all the pixels that correspond to a nonzero binary
//! mask value and picking the smallest one.
//!
//! @param src The heightmap source subimage
//! @param mask The binary droplet dection mask
unsigned char find_min_height(cv::Mat & src, cv::Mat & mask, int bgHeight);

//! @brief Calculates a target's volume
//!
//! This function works exactly like find_area, except that it integrates
//! src pixels rather than mask pixels.
//!
//! @param src The heightmap source subimage
//! @param mask The binary droplet dection mask
long find_volume(cv::Mat & src, cv::Mat & mask, int bgHeight);

//! @brief Calculates a target's average height
//!
//! This function works calculates the average height of a target.
//!
//! @param src The heightmap source subimage
//! @param mask The binary droplet dection mask
long find_average_height(cv::Mat & src, cv::Mat & mask, int bgHeight);


double find_circularity(cv::Mat & mask);
