#pragma once
#include <cstddef>
#include <vector>

#include "Colours.h"
#include "Text.h"

struct TextTag {
  // inclusive
  size_t m_start_row;
  size_t m_start_col;
  // exclusive
  size_t m_end_row;
  size_t m_end_col;

  COLOUR m_colour;

  TextTag(size_t start_row, size_t start_col, COLOUR colour)
      : m_start_row(start_row), m_start_col(start_col), m_end_row(start_row),
        m_end_col(start_col), m_colour(colour) {
  }

  TextTag(size_t start_row, size_t start_col, size_t end_row, size_t end_col,
          COLOUR colour)
      : m_start_row(start_row), m_start_col(start_col), m_end_row(end_row),
        m_end_col(end_col), m_colour(colour) {
  }
};
