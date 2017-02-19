#pragma once

#include <array>
#include <opencv2/imgproc/imgproc.hpp>

inline cv::Rect make_cv_roi(const std::array<double, 4> & backend_roi_fmt,
                            const cv::Mat & src) {
    const int startx = (backend_roi_fmt[0]) * src.cols;
    const int starty = (backend_roi_fmt[1]) * src.rows;
    const int width = (backend_roi_fmt[2] - backend_roi_fmt[0]) * src.cols;
    const int height = (backend_roi_fmt[3] - backend_roi_fmt[1]) * src.rows;
    return cv::Rect(startx, starty, width, height);
}
