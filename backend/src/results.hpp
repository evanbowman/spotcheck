//! @file
//! @brief A class that encapsulates and serializes result data.

#pragma once

#include <iostream>

//! @class Result
class Result {
    int m_rowId, m_colId;
    int m_bgHeight;
    int m_area;
    unsigned char m_min_height, m_max_height;
    long m_volume;
    long m_average_height;
    double m_circularity;

public:
    Result(int rowId, int colId, int bgHeight, int area,
           unsigned char min_height, unsigned char max_height, long volume,
           long average_height, double circularity);

    void serialize(std::ostream & target) const;
};
