#include "parse_gal.hpp"

enum class parse_result { success, failure };

inline static parse_result verify_encoding(std::fstream & file,
                                           std::string & line) {
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string encoding, version;
        if (!(iss >> encoding >> version)) {
            return parse_result::failure;
        } else {
            if (encoding != "ATF") {
                return parse_result::failure;
            }
        }
    } else {
        return parse_result::failure;
    }
    return parse_result::success;
}

inline static parse_result get_header_metadata(std::fstream & file,
                                               std::string & line,
                                               int & header_lines) {
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        int data_columns;
        if (!(iss >> header_lines >> data_columns)) {
            return parse_result::failure;
        }
    } else {
        return parse_result::failure;
    }
    return parse_result::success;
}

option<gal_data> parse_gal(std::fstream & file) {
    gal_data results;
    std::string line;
    if (verify_encoding(file, line) == parse_result::failure) {
        return {};
    }
    int header_lines = 0;
    if (get_header_metadata(file, line, header_lines) ==
        parse_result::failure) {
        return {};
    }
    std::regex keywords("BlockCount|BlockType|Supplier|(Block[0-9]+)|Type");
    std::regex numerals("[0-9]+");
    std::smatch numeral_matches;
    for (int i = 0; i < header_lines; i += 1) {
        if (std::getline(file, line)) {
            std::smatch matches;
            if (std::regex_search(line, matches, keywords)) {
                if (matches[0] == "BlockCount") {
                    if (std::regex_search(line, numeral_matches, numerals)) {
                        const size_t block_count =
                            std::stoi(numeral_matches[0]);
                        results.m_blocks.resize(block_count);
                    } else {
                        return {};
                    }
                } else if (matches[0] == "BlockType") {
                    if (std::regex_search(line, numeral_matches, numerals)) {
                        results.m_block_type = std::stoi(numeral_matches[0]);
                    } else {
                        return {};
                    }
                } else if (matches[0] == "Type" || matches[0] == "Supplier") {
                    // Ignore...
                } else {
                    std::string block_name(matches[0]);
                    if (std::regex_search(block_name, numeral_matches,
                                          numerals)) {
                        const size_t block_number =
                            std::stoi(numeral_matches[0]);
                        if (results.m_blocks.size() >= block_number) {
                            size_t equal_sign_pos = line.find("=");
                            if (equal_sign_pos != std::string::npos) {
                                auto rhs = line.substr(equal_sign_pos + 1,
                                                       line.length() - 1);
                                std::stringstream ss(rhs);
                                int current, i = 0;
                                while (ss >> current) {
                                    if (i > 6) {
                                        return {};
                                    }
                                    results.m_blocks[block_number - 1].info[i] =
                                        current;
                                    i += 1;
                                    if (ss.peek() == ',') {
                                        ss.ignore();
                                    }
                                }
                            } else {
                                return {};
                            }
                        } else {
                            return {};
                        }
                    } else {
                        return {};
                    }
                }
            } else {
                return {};
            }
        } else {
            return {};
        }
    }
    if (!std::getline(file, line)) {
        return {};
    }
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int bloc, row, col;
        std::string id, name;
        if (!(iss >> bloc >> row >> col >> id >> name)) {
            return {};
        } else {
            results.m_blocks[bloc - 1].data[std::make_pair(row - 1, col - 1)] =
                name;
        }
    }
    return std::move(results);
}

gal_data::gal_data() : m_block_type(0) {}

size_t gal_data::num_blocks() const { return m_blocks.size(); }

const gal_block & gal_data::get_block(const size_t index) const {
    return m_blocks[index];
}
