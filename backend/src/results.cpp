#include "results.hpp"

void Result::add_data(const std::pair<std::string, double> & data) {
    m_keyValuePairs.push_back(data);
}

void Result::serialize(std::ostream & ostr) const {
    std::string out;
    out += "{";
    for (auto & kvp : m_keyValuePairs) {
        out += "\"" + kvp.first + "\":" + std::to_string(kvp.second) + ",";
    }
    // Note: some JSON parsers don't like there to be a comma with
    // no data after it, so pop it off the string.
    out.pop_back();
    out += "}";
    ostr << out;
}
