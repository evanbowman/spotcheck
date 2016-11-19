#include "test_thresh.hpp"

extern std::string module_path;

#define SCALE 2

static int kernel_size = 3;

void test_thresh(const cv::Mat & src, const uint8_t thresh,
                 const bool draw_circles,
                 const std::array<int, 4> & backend_roi) {
    auto cv_roi = make_cv_roi(backend_roi, src);
    cv::Mat src_roi = src(cv_roi);
    cv::Mat src_gray, edges;
    cv::cvtColor(src_roi, src_gray, CV_BGR2GRAY);
    normalize(cv::InputArray(src_gray), cv::InputOutputArray(src_gray), 0, 255,
              cv::NORM_MINMAX, CV_8UC1);
    cv::threshold(src_gray, src_gray, thresh, 255, 3);
    cv::Mat blurred;
    cv::GaussianBlur(src_gray, blurred, cv::Size(9, 9), 4, 4);
    cv::pyrDown(blurred, blurred,
                cv::Size(src_roi.cols / SCALE, src_roi.rows / SCALE));
    cv::pyrDown(src_gray, src_gray,
                cv::Size(src_roi.cols / SCALE, src_roi.rows / SCALE));
    cv::Canny(blurred, edges, 100, 300, ::kernel_size);
    // put a threshold on the canny results to improve the search later
    cv::threshold(edges, edges, 6, 255, 0);
    // do a Hough transform to find the places where the dots are
    std::vector<cv::Vec3f> circles;
    cv::Mat tmp;
    GaussianBlur(edges, tmp, cv::Size(9, 9), 2, 2);
    static const int high = 70;
    static const int low = 50;
    cv::Mat h_out;
    cvtColor(src_gray, h_out, CV_GRAY2RGB);
    if (draw_circles) {
        HoughCircles(tmp, circles, CV_HOUGH_GRADIENT, 1, tmp.rows / 8, high,
                     low, 0, 0);
        for (size_t i = 0; i < circles.size(); i += 1) {
            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            cv::circle(h_out, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
            cv::circle(h_out, center, radius, cv::Scalar(0, 32, 255), 3, 8, 0);
        }
    }
    const std::string output_dir =
        ::module_path + "/../../../frontend/temp/tmp.png";
    cv::imwrite(output_dir, h_out);
}
