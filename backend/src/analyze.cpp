#include "analyze.hpp"

static const int SCALE = 2;
static const int rad_pad = 10;
static const int kernel_size = 3;

std::vector<spot> find_spots(const cv::Mat & src, const uint8_t thresh,
                             std::array<int, 4> & backend_roi) {
    auto cv_roi = make_cv_roi(backend_roi, src);
    cv::Mat src_cropped = src(cv_roi);
    cv::Mat src_gray, edges;
    cv::cvtColor(src_cropped, src_gray, CV_BGR2GRAY);
    cv::normalize(cv::InputArray(src_gray), cv::InputOutputArray(src_gray), 0,
                  255, cv::NORM_MINMAX, CV_8UC1);
    cv::threshold(src_gray, src_gray, thresh, 255, 3);
    cv::GaussianBlur(src_gray, src_gray, cv::Size(9, 9), 4, 4);
    cv::pyrDown(src_gray, src_gray,
                cv::Size(src_cropped.cols / SCALE, src_cropped.rows / SCALE));
    cv::Canny(src_gray, edges, 100, 300, kernel_size);
    // put a threshold on the canny results to improve the search later
    cv::threshold(edges, edges, 6, 255, 0);
    // do a Hough transform to find the places where the dots are
    std::vector<cv::Vec3f> circles;
    cv::Mat tmp;
    GaussianBlur(edges, tmp, cv::Size(9, 9), 2, 2);
    static const int high = 70;
    static const int low = 50;
    HoughCircles(tmp, circles, CV_HOUGH_GRADIENT, 1, tmp.rows / 8, high, low, 0,
                 0);
    std::vector<spot> results;
    const size_t num_circ = circles.size();
    for (size_t i = 0; i < num_circ; i += 1) {
        int x, y, r;
        // circle centers & radius
        x = cvRound(circles[i][0]);
        y = cvRound(circles[i][1]);
        r = cvRound(circles[i][2]) + rad_pad;
        results.emplace_back(std::make_pair(x, y));
        double xbar = 0;
        double ybar = 0;
        double dbar = 0;
        int count = 0;
        // check all the pixels in a square 2r by 2r for the white boundry
        for (int row = y - r; row < (y + r); row += 1) {
            for (int col = x - r; col < (x + r); col += 1) {
                // if the pixel is white, the its the boundry found by canny
                cv::Scalar i = edges.at<unsigned char>(row, col);
                if (i.val[0] == 255) {
                    // get the distance of the point from the center
                    results.back().get_edge().push_back(cv::Point(col, row));
                    count++;
                    dbar += sqrt(pow(col - x, 2) + pow(row - y, 2));
                    xbar += col;
                    ybar += row;
                }
            }
        }
        results.back().set_bar(std::make_pair(xbar / count, ybar / count));
        results.back().set_avg_r(dbar / count);
    }
    return results;
}

void circ_score(std::vector<spot> & spots) {
    // calculate circle score
    // the Hough centers are pretty close to xbar and ybar
    // but i decided to use them for extra accuracy
    // though the improvement is marginal and can be taken out later
    double e;
    const int num_circ = spots.size();
    int numPnt;
    double xbar;
    double ybar;
    double dbar;
    for (int i = 0; i < num_circ; i++) {
        numPnt = spots[i].get_edge().size();
        xbar = spots[i].get_bar().first;
        ybar = spots[i].get_bar().second;
        dbar = spots[i].get_avg_r();
        e = 1.0;
        for (int j = 0; j < numPnt; j++) {
            e *= sqrt(pow(spots[i].get_edge()[j].x - xbar, 2) +
                      pow(spots[i].get_edge()[j].y - ybar, 2));
            e /= dbar;
        }
        spots[i].set_circ_score(e);
    }
}
