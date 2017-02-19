#include "results.hpp"

result::result(int rowId, int colId, int bgHeight, int area,
               unsigned char min_height, unsigned char max_height, long volume,
               long average_height)
    : m_rowId(rowId), m_colId(colId), m_bgHeight(bgHeight), m_area(area),
      m_min_height(min_height), m_max_height(max_height), m_volume(volume),
      m_average_height(average_height) {}

void result::serialize(std::ostream & ostr) const {
    ostr << "{"
         << "\"row\":"
         << "\"" << m_rowId << "\","
         << "\"col\":"
         << "\"" << m_colId << "\","
         << "\"area\":"
         << "\"" << m_area << "\","
         << "\"min height\":"
         << "\"" << static_cast<int>(m_min_height) << "\","
         << "\"max height\":"
         << "\"" << static_cast<int>(m_max_height) << "\","
         << "\"avg height\":"
         << "\"" << m_average_height << "\","
         << "\"volume\":"
         << "\"" << m_volume << "\"}";
}
