#include <algorithm>
#include <cstdint>

struct Cursor {
  size_t m_row;
  size_t m_col;

  size_t m_original_col;

  Cursor(size_t row, size_t col, size_t original_col)
      : m_row(row), m_col(col), m_original_col(original_col) {
  }

  void move_to_new_line(size_t new_line_length) {
    m_col = std::min(new_line_length, m_original_col);
  }

  void reset_original_col() {
    m_original_col = m_col;
  }
};