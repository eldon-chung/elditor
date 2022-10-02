#pragma once
#include <algorithm>
#include <cstdint>

class CursorPoint {
  size_t m_row;
  size_t m_col;
  size_t m_original_col;

public:
  CursorPoint(size_t row, size_t col, size_t original_col)
      : m_row(row), m_col(col), m_original_col(original_col) {
  }

  size_t &row() {
    return m_row;
  }

  size_t &col() {
    return m_col;
  }

  size_t &original_col() {
    return m_original_col;
  }

  void move_to_row(size_t row) {
    m_row = row;
  }

  void move_to_col(size_t col) {
    m_col;
  }

  void reset_to_col(size_t col) {
    m_col = col;
    m_original_col = col;
  }

  size_t row() const {
    return m_row;
  }

  size_t col() const {
    return m_col;
  }

  size_t original_col() const {
    return m_original_col;
  }

  void reset_original_col() {
    m_col = m_original_col;
  }

  bool operator==(CursorPoint const &other) const {
    return m_original_col == other.original_col() && m_col == other.col() && m_row == other.row();
  }

  bool in_same_place(CursorPoint const &other) const {
    return m_col == other.col() && m_row == other.row();
  }

  bool is_behind(CursorPoint const &other) const {
    return m_row < other.row() || (m_row == other.row() && m_col < other.col());
  }
};

class Cursor {
  CursorPoint m_active_point;
  CursorPoint m_trailing_point;

public:
  Cursor(size_t row, size_t col, size_t original_col)
      : m_active_point{row, col, original_col}, m_trailing_point{m_active_point} {
  }

  CursorPoint &active_point() {
    return m_active_point;
  }

  CursorPoint &trailing_point() {
    return m_trailing_point;
  }

  bool in_selection_mode() const {
    // we are not selecting place if and only if they are currently sitting in the same place
    return m_active_point.in_same_place(m_trailing_point);
  }

  bool active_point_is_behind_trailing_point() {
    return m_active_point.is_behind(m_trailing_point);
  }

  void reset_original_col() {
    m_active_point.reset_original_col();
  }

  // Gets the row of cursor's active point
  size_t &row() {
    return m_active_point.row();
  }

  // Gets the col of cursor's active point
  size_t &col() {
    return m_active_point.col();
  }

  // Gets the original_col of cursor's active point
  size_t &original_col() {
    return m_active_point.original_col();
  }

  // Gets the row of cursor's active point
  size_t row() const {
    return m_active_point.row();
  }

  // Gets the col of cursor's active point
  size_t col() const {
    return m_active_point.col();
  }

  // Gets the original_col of cursor's active point
  size_t original_col() const {
    return m_active_point.original_col();
  }

  // Gets the row of cursor's trailing point
  size_t trailing_row() const {
    return m_trailing_point.row();
  }

  // Gets the col of cursor's trailing point
  size_t trailing_col() const {
    return m_trailing_point.col();
  }

  // Gets the original_col of cursor's trailing point
  size_t original_col() const {
    return m_trailing_point.original_col();
  }
};