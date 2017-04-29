//! @file
//! @brief Definition for a class that encapsulates and serializes result data.

#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <vector>

//! @class Result
//! @brief A class that encapsulate a result json object.
class Result {
    std::vector<std::pair<std::string, double>> m_keyValuePairs;

public:
    //! @brief Adds a JSON key value pair to the Result
    void add_data(const std::pair<std::string, double> & data);

    //! @brief Writes a JSON representation of Result to a std::ostream
    //!
    //! @param target Any output stream format, e.g. std::cout, std::fstream,
    //! etc.
    void serialize(std::ostream & target) const;
};
