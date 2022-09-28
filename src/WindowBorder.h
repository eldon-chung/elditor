#pragma once

#include <cassert>

class WindowBorder {
  int m_starting_col;
  int m_starting_row;
  int m_height;
  int m_width;

public:
  WindowBorder(int height, int width)
      : m_starting_col(0), m_starting_row(0), m_height(height), m_width(width) {
  }

  int starting_row() const {
    return m_starting_row;
  }

  int ending_row() const {
    return m_starting_row + m_height;
  }

  int starting_col() const {
    return m_starting_col;
  }

  int ending_col() const {
    return m_starting_col + m_width;
  }

  int height() const {
    return m_height;
  }

  int width() const {
    return m_width;
  }

  void move_right(int delta) {
    m_starting_row += delta;
  }

  void move_left(int delta) {
    // should really throw an exception
    assert(m_starting_row >= delta);
    m_starting_row -= delta;
  }

  void move_up(int delta) {
    // should really throw an exception
    assert(m_starting_col >= delta);
    m_starting_col -= delta;
  }

  void move_down(int delta) {
    m_starting_col += delta;
  }

  void chase_point(int row, int col) {
    // 0 1 2 3 | 4
    if (col >= m_starting_col + m_width) {
      m_starting_col = col + 1 - m_width;
    } else if (col < m_starting_col) {
      m_starting_col = col;
    }

    if (row >= m_starting_row + m_height) {
      m_starting_row = row + 1 - m_height;
    } else if (row < m_starting_row) {
      m_starting_row = row;
    }
  }
};