#include "circ_score.hpp"

#define SCALE 2

static int rad_pad = 10;
static int kernel_size = 3;

// TODO: this will be user adjustable
static const int thresh = 170;

std::vector<double> circ_score(const cv::Mat & src) {
    cv::Mat src_gray, edges;
    cv::cvtColor(src, src_gray, CV_BGR2GRAY);
    cv::normalize(cv::InputArray(src_gray), cv::OutputArray(src_gray), 0, 255,
                  cv::NORM_MINMAX, CV_8UC1);
    cv::threshold(src_gray, src_gray, thresh, 255, 3);
    cv::GaussianBlur(src_gray, src_gray, cv::Size(9, 9), 4, 4);
    cv::pyrDown(src_gray, src_gray,
                cv::Size(src.cols / SCALE, src.rows / SCALE));
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
    std::vector<double> ans;
    // now that we know the number of circles found, we can allocate the output
    // array
    // the first element will be the number of circles found, so we can use
    // that info later
    const size_t num_circ = circles.size();
    // creating a copy to put the cirlces that the Hough trans find
    cv::Mat h_out;
    cvtColor(edges, h_out, CV_GRAY2RGB);

    for (size_t i = 0; i < num_circ; i++) {
        int x, y, r, count;
        // circle centers & radius
        x = cvRound(circles[i][0]);
        y = cvRound(circles[i][1]);
        r = cvRound(circles[i][2]) + rad_pad;

        // these doubles will hold the centers and average distance from the
        // edge
        // to the Hough center
        double xbar = 0;
        double ybar = 0;
        double dbar = 0;
        count = 0;
        // check all the pixels in a square 2r by 2r for the white boundry
        for (int row = y - r; row < (y + r); row++) {
            for (int col = x - r; col < (x + r); col++) {
                // if the pixel is white, the its the boundry found by canny
                cv::Scalar i = edges.at<unsigned char>(row, col);
                // std::cout <<"ival " << i.val[0]<< std::endl;
                if (i.val[0] == 255) {
                    // printf( "%d\n", i.val[0] );
                    // get the distance of the point from the center
                    count++;
                    dbar += sqrt(pow(col - x, 2) + pow(row - y, 2));
                    xbar += col;
                    ybar += row;
                }
            }
        }
        xbar /= count;
        ybar /= count;
        dbar /= count;

        // calculate circle score
        // the Hough centers are pretty close to xbar and ybar
        // but i decided to use them for extra accuracy
        // though the improvement is marginal and can be taken out later
        double e = 1;
        for (int row = y - r; row < (y + r); row++) {
            for (int col = x - r; col < (x + r); col++) {

                cv::Scalar i = edges.at<unsigned char>(row, col);
                if (i.val[0] == 255) {
                    // for each point, find it ratio to the average distace
                    // the score is the product for all points
                    e *= sqrt(pow(col - xbar, 2) + pow(row - ybar, 2));
                    e /= dbar;
                }
            }
        }
        ans.push_back(e);

        cv::Point center(x, y);
        // draw the circles on h_out
        circle(h_out, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
        circle(h_out, center, r, cv::Scalar(255, 0, 0), 3, 8, 0);

        // draw the circle score on the output image
        // convert e to a string
        char buff[50];
        sprintf(buff, "%f", e);
        std::string s(buff);
        putText(h_out, s, center, cv::FONT_HERSHEY_SIMPLEX, 0.5,
                cv::Scalar(0, 0, 255), 1, 8);
    }
    for (auto value : ans) {
        std::cout << value << std::endl;
    }
    return ans;
};
