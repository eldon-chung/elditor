#include <cstdint>

struct Cursor {
  std::size_t m_row;
  std::size_t m_col;

  Cursor(std::size_t row, std::size_t col) : m_row(row), m_col(col) {
  }
};