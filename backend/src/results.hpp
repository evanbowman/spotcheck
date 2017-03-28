//! @file
//! @brief Definition for a class that encapsulates and serializes result data.

#pragma once

#include <iostream>

//! @class Result
//! @brief A class that encapsulates and serializes result data.
class Result {
    int m_rowId, m_colId;
    int m_bgHeight;
    int m_area;
    double m_min_height, m_max_height;
    double m_volume;
    double m_average_height;
    double m_circularity;

public:
    Result(int rowId, int colId, double bgHeight, double area,
           double min_height, double max_height, double volume,
           double average_height, double circularity);

    //! @brief Writes a JSON representation of Result to a std::ostream
    //!
    //! @param target Any output stream format, e.g. std::cout, std::fstream,
    //! etc.
    void serialize(std::ostream & target) const;
};
