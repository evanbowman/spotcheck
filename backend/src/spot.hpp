#pragma once

#include "opencv2/imgproc/imgproc.hpp"
#include <utility>

class spot {
public:	
    explicit spot(const std::pair<int, int> &);
    void info();
    std::pair<int, int> get_coord() const;
    const std::pair<double, double> & get_bar() const;
    double get_avg_r() const;
    std::vector<cv::Point> & get_edge();
    double get_circ_score() const;
    double get_avg_height() const;
    double get_std_height() const;
    double get_volume() const;
    void set_bar(const std::pair<double, double> &);
    void set_circ_score(const double);
    void set_avg_height(const double);
    void set_std_height(const double);
    void set_volume(const double);
private:
    std::pair<int, int> m_coord;
    std::pair<double, double> m_bar;
    double m_avg_r;
    std::vector<cv::Point> m_edge;
    double m_circ_score;
    double m_avg_height;
    double m_std_height;
    double m_volume;
};
