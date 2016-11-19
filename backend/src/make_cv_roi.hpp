#pragma once

#include <array>
#include <opencv2/imgproc/imgproc.hpp>

inline cv::Rect make_cv_roi(const std::array<int, 4> & backend_roi_fmt,
                            const cv::Mat & src) {
    const int startx = (backend_roi_fmt[0] / 100.f) * src.cols;
    const int starty = (backend_roi_fmt[1] / 100.f) * src.rows;
    const int width =
        ((backend_roi_fmt[2] - backend_roi_fmt[0]) / 100.f) * src.cols;
    const int height =
        ((backend_roi_fmt[3] - backend_roi_fmt[1]) / 100.f) * src.rows;
    return cv::Rect(startx, starty, width, height);
}
