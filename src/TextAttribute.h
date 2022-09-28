#pragma once

#include <cstdint>
#include <ncurses.h>

#include "Colours.h"

struct TextAttribute {
  int m_row;
  int m_col;
  int m_length;
  attr_t m_attribute;
  short m_colour;

  TextAttribute(int row, int col, int length, attr_t attribute, short colour)
      : m_row(row), m_col(col), m_length(length), m_attribute(attribute),
        m_colour(colour) {
  }

  int x() {
    return m_col;
  }

  int y() {
    return m_row;
  }

  int n() {
    return m_length;
  }

  attr_t attribute() {
    return m_attribute;
  }

  short colour() {
    return m_colour;
  }
};