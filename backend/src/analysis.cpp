#include "analysis.hpp"

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
// threshold for canny edge detection
// const int THRESH = 50;
const double PI = 3.141592653589793238463;

// debug
extern std::string module_path;
///

cv::RNG rng(12345);

int find_background(cv::Mat & src, cv::Mat & mask) {
    long sum = 0;
    int quant = 0;
    for (int i = 0; i < mask.rows; ++i) {
        for (int j = 0; j < mask.cols; ++j) {
            if (mask.at<unsigned char>(i, j) == 0) {
                sum += src.at<unsigned char>(i, j);
                quant++;
            }
        }
    }
    return sum / quant;
}

int find_area(cv::Mat & src, cv::Mat & mask) {
    int quant = 0;
    for (int i = 0; i < mask.rows; ++i) {
        for (int j = 0; j < mask.cols; ++j) {
            if (mask.at<unsigned char>(i, j) > 0) {
                quant++;
            }
        }
    }
    return quant;
}

unsigned char find_max_height(cv::Mat & src, cv::Mat & mask, int bgHeight) {
    unsigned char max = 0;
    for (int i = 0; i < mask.rows; ++i) {
        for (int j = 0; j < mask.cols; ++j) {
            if (mask.at<unsigned char>(i, j) > 0) {
                if (max < mask.at<unsigned char>(i, j)) {
                    max = mask.at<unsigned char>(i, j) - bgHeight;
                }
            }
        }
    }
    return max;
}

unsigned char find_min_height(cv::Mat & src, cv::Mat & mask, int bgHeight) {
    unsigned char min = 255;
    for (int i = 0; i < mask.rows; ++i) {
        for (int j = 0; j < mask.cols; ++j) {
            if (mask.at<unsigned char>(i, j) > 0) {
                if (min > mask.at<unsigned char>(i, j)) {
                    min = mask.at<unsigned char>(i, j) - bgHeight;
                }
            }
        }
    }
    return min;
}

long find_volume(cv::Mat & src, cv::Mat & mask, int bgHeight) {
    long volume = 0;

    for (int i = 0; i < mask.rows; ++i) {
        for (int j = 0; j < mask.cols; ++j) {
            if (mask.at<unsigned char>(i, j) > 0) {
                volume += src.at<unsigned char>(i, j) - bgHeight;
            }
        }
    }

    return volume;
}

long find_average_height(cv::Mat & src, cv::Mat & mask, int bgHeight) {
    long volume = 0;
    long quant = 0;

    for (int i = 0; i < mask.rows; ++i) {
        for (int j = 0; j < mask.cols; ++j) {
            if (mask.at<unsigned char>(i, j) > 0) {
                volume += src.at<unsigned char>(i, j) - bgHeight;
                quant++;
            }
        }
    }

    return volume / quant;
}

double find_circularity(cv::Mat & mask) {

    // Do canny to get the edges of the mask,
    cv::Mat canny_output;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    // SEEMS TO WORK
    // COULD TUNE THRESHOLDS
    cv::Canny(mask, canny_output, 0, 255, 3);

    // contourFind modifies the input for some reason
    auto contourFind = canny_output;

    // find countours
    // cv::findContours(contourFind, contours, hierarchy, CV_RETR_CCOMP,
    //  CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
    cv::findContours(contourFind, contours, hierarchy, CV_RETR_LIST,
                     CV_CHAIN_APPROX_NONE, cv::Point(0, 0));

    std::cout << "found " << contours.size() << "contours" << std::endl;

    // / Draw contours
    // cv::Mat drawing = cv::Mat::zeros(contourFind.size(), CV_8UC3);
    // for (int i = 0; i < contours.size(); i++) {
    //     cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0,
    //     255),
    //                                   rng.uniform(0, 255));
    //     cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0,
    //                      cv::Point());
    // }
    ///////////
    // Debug Code write to file
    std::cout << ::module_path << std::endl;
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    char num[8];
    int rnd = rng.uniform(0, 100);
    // std::cout << rnd << std::endl;
    sprintf(num, "%d", rnd);

    static const auto extension = ".png";
    const std::string fname =
        ::module_path + "/../../../frontend/temp/contours_" + num + extension;
    std::cout << fname << std::endl;

    /* ID by max area fails, did by perimeter instead
    // can delete this block, left in for future ref
    //////////////
    // need to pick a contour
    // assume the spot is the contour of largest area
    // use it to compute circularity.

    // std::cout << "IMAGE NUM " << rnd << "\n";
    // double maxArea = 0;
    // int maxCont = 0;
    // for (uint i = 0; i < contours.size(); i++) {
    //     // std::cout << contours[i] << std::endl;
    //     double contAr = contourArea(contours[i], false);
    //     if (maxArea < contAr) {
    //         maxArea = contAr;
    //         maxCont = i;
    //     }
    //     printf("countour: %d, area: %f\n", i, contAr);
    // }
    */

    /* try by contour perimeter instead.*/
    std::cout << "IMAGE NUM " << rnd << "\n";
    double maxPerim = 0;
    int maxCont = 0;
    for (uint i = 0; i < contours.size(); i++) {
        // std::cout << contours[i] << std::endl;
        double contAr = cv::arcLength(contours[i], true);
        if (maxPerim < contAr) {
            maxPerim = contAr;
            maxCont = i;
        }
        printf("countour: %d, area: %f\n", i, contAr);
    }

    // cv::imwrite(fname, canny_output);
    // cv::imwrite(fname, drawing);

    //

    auto area = cv::contourArea(contours[maxCont], false);
    auto perimeter = cv::arcLength(contours[maxCont], true);

    std::cout << "cvarea " << area << std::endl;
    std::cout << "cvperim " << perimeter << std::endl;

    // circularity
    // https://en.wikipedia.org/wiki/Shape_factor_%28image_analysis_and_microscopy%29
    // f_circ = 4*pi*Area/(perimeter^2)
    // 1 for circle, 0 less

    double circ = (4 * PI * area) / (perimeter * perimeter);
    std::cout << "circ " << circ << std::endl;

    cv::Scalar RED = cv::Scalar(0, 0, 255);

    // Draw contours
    cv::Mat drawing = cv::Mat::zeros(contourFind.size(), CV_8UC3);
    for (uint i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
                                      rng.uniform(0, 255));
        cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0,
                         cv::Point());
    }

    // largest drawn in red
    cv::drawContours(drawing, contours, maxCont, RED, 2, 8, hierarchy, 0,
                     cv::Point());

    cv::imwrite(fname, drawing);

    return circ;
    // return 0;
}
