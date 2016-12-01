#include "spot.hpp"
#include <stdio.h>

spot::spot(const std::pair<int, int> & coord)
    : m_coord(coord), m_bar{}, m_avg_r(0.0), m_circ_score(0.0),
      m_avg_height(0.0), m_std_height(0.0), m_volume(0.0) {}

std::pair<int, int> spot::get_coord() const { return m_coord; }

const std::pair<double, double> & spot::get_bar() const { return m_bar; }

double spot::get_avg_r() const { return m_avg_r; }

void spot::set_avg_r(const double avg_r) { m_avg_r = avg_r; }

std::vector<cv::Point> & spot::get_edge() { return m_edge; }

double spot::get_circ_score() const { return m_circ_score; }

double spot::get_avg_height() const { return m_avg_height; }

double spot::get_volume() const { return m_volume; }

void spot::set_bar(const std::pair<double, double> & bar) { m_bar = bar; }

void spot::set_circ_score(const double circ_score) {
    m_circ_score = circ_score;
}

void spot::set_avg_height(const double avg_height) {
    m_avg_height = avg_height;
}

void spot::set_std_height(const double std_height) {
    m_std_height = std_height;
}

void spot::set_volume(const double volume) { m_volume = volume; }

void spot::serialize(std::ostream & ostr) const {
    ostr << "{"
         << "\"xCenter\":" << "\"" << m_coord.first << "\"" << ","
         << "\"yCenter\":" << "\"" << m_coord.second << "\"" << ","
         << "\"circScore\":" << "\"" << m_circ_score << "\"" << ","
         << "\"averageRad\":" << "\"" << m_avg_r << "\"" << ","
         << "\"averageHeight\":" << "\"" << m_avg_height << "\"" << ","
         << "\"stdHeight\":" << "\"" << m_std_height << "\"" << ","
         << "\"volume\":" << "\"" << m_volume << "\""
         << "}";
}
