#pragma once
#include <iostream>
class result{
  int m_rowId, m_colId;
  int m_bgHeight;
  int m_area;
  unsigned char m_min_height, m_max_height;
  long m_volume;
  long m_average_height;

public:
  result(int rowId, int colId, int bgHeight, int area,
     unsigned char min_height, unsigned char max_height, long volume, long average_height);

  void serialize(std::ostream& target);


};
