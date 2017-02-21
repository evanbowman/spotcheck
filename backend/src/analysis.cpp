#include "analysis.hpp"

// threshold for canny edge detection
// const int THRESH = 50;
static const double PI = 3.141592653589793238463;

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
    return sum / std::max(quant, 1);
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
                if (max < src.at<unsigned char>(i, j) - bgHeight) {
                    max = src.at<unsigned char>(i, j) - bgHeight;
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
                if (min > src.at<unsigned char>(i, j) - bgHeight) {
                    min = src.at<unsigned char>(i, j) - bgHeight;
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

    return volume / std::min(1l, quant);
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

    cv::findContours(contourFind, contours, hierarchy, CV_RETR_LIST,
                     CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
    
    double maxPerim = 0;
    int maxCont = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double contAr = cv::arcLength(contours[i], true);
        if (maxPerim < contAr) {
            maxPerim = contAr;
            maxCont = i;
        }
    }

    auto area = cv::contourArea(contours[maxCont], false);
    auto perimeter = cv::arcLength(contours[maxCont], true);

    // circularity
    // https://en.wikipedia.org/wiki/Shape_factor_%28image_analysis_and_microscopy%29
    // f_circ = 4*pi*Area/(perimeter^2)
    // 1 for circle, 0 less

    double circ = (4 * PI * area) / (perimeter * perimeter);

    cv::Scalar RED = cv::Scalar(0, 0, 255);
    cv::Mat drawing = cv::Mat::zeros(contourFind.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
                                      rng.uniform(0, 255));
        cv::drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0,
                         cv::Point());
    }

    // largest drawn in red
    cv::drawContours(drawing, contours, maxCont, RED, 2, 8, hierarchy, 0,
                     cv::Point());
    return circ;
}
