#pragma once

#include <algorithm>
#include <cstring>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "option.hpp"

struct gal_block {
    std::array<int, 7> info;
    std::map<std::pair<int, int>, std::string> data;
};

class gal_data {
public:
    gal_data();
    friend option<gal_data> parse_gal(std::fstream & file);
    size_t num_blocks() const;
    const gal_block & get_block(const size_t index) const;

private:
    std::vector<gal_block> m_blocks;
    int m_block_type;
};

option<gal_data> parse_gal(std::fstream & file);
