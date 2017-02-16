#include "preview_normalized.hpp"

extern std::string module_path;

void preview_normalized(const cv::Mat & src) {
    cv::Mat src_gray, src_norm;
    cv::cvtColor(src, src_gray, CV_BGR2GRAY);
    normalize(cv::InputArray(src_gray), cv::OutputArray(src_norm), 0, 255, cv::NORM_MINMAX, CV_8UC1);
    const std::string output_dir = ::module_path +
	"/../../../frontend/temp/tmp.png";
    cv::imwrite(output_dir, src_norm);
}
