#pragma once
#include <cassert>
#include <deque>
#include <ncurses.h>
#include <string>
#include <vector>

#include "Cursor.h"
#include "TextAttribute.h"

struct TextWindow {
  WINDOW *m_window_ptr;
  std::deque<std::string> m_lines;
  std::vector<TextAttribute> m_text_attributes;
  // left_boundary
  size_t m_left_boundary;
  // height of the screen
  size_t m_num_rows;
  // width of the screen
  size_t m_num_cols;

  // if the boundaries are not defined, we delegate its construction
  TextWindow(WINDOW *window_ptr, size_t num_rows, size_t num_cols)
      : TextWindow(window_ptr, num_rows, num_cols, 0) {
  }

  TextWindow(WINDOW *window_ptr, size_t num_rows, size_t num_cols,
             size_t left_boundary)
      : m_window_ptr(window_ptr), m_left_boundary(left_boundary),
        m_num_rows(num_rows), m_num_cols(num_cols) {
    for (size_t row = 0; row < m_num_rows; row++) {
      m_lines.push_back(std::string(""));
    }
  }

  void update(std::vector<std::string_view> &&new_contents) {
    assert(new_contents.size() == m_num_rows);
    m_text_attributes.clear();
    m_lines.clear();
    for (std::string_view &s : new_contents) {
      // std::cerr << "TextWindow: Updating line \"" << s << "\"" << std::endl;
      m_lines.push_back(std::string(s));
    }
    new_contents.clear();
    // subsequently add in the attributes as well
  }

  void set_left_boundary(size_t left_boundary) {
    m_left_boundary = left_boundary;
  }

  void render() {
    // clear the current screen
    werase(m_window_ptr);
    // get a "string_view" for each row and place it onto the screen
    for (size_t row_idx = 0; row_idx < m_num_rows; row_idx++) {
      if (m_left_boundary >= m_lines.at(row_idx).size()) {
        // if the starting column is too far right then we dont have to bother
        // std::cerr << "TextWindow: Rendering empty line" << std::endl;
        mvwaddstr(m_window_ptr, row_idx, 0, "");
      } else {
        // else there get a view of the current string, starting from start_col
        const char *start_ptr = m_lines.at(row_idx).data() + m_left_boundary;
        std::string_view rendered_line{start_ptr, m_num_cols};
        // std::cerr << "TextWindow: Rendering " << rendered_line << std::endl;
        mvwaddstr(m_window_ptr, row_idx, 0, rendered_line.data());
      }
    }

    // place the attributes on the screen
    wstandend(m_window_ptr);
    for (TextAttribute &text_attribute : m_text_attributes) {
      mvwchgat(m_window_ptr, text_attribute.y(), text_attribute.x(),
               text_attribute.n(), text_attribute.attribute(),
               text_attribute.colour(), NULL);
    }

    // mvwchgat(m_window_ptr, 0, 0, 1, A_STANDOUT, 0, NULL);
    // waddch('c');

    wrefresh(m_window_ptr);
  }

  size_t height() const {
    return m_num_rows;
  }

  void add_attribute(int y, int x, int length, attr_t attribute, short color) {
    // translate this into an attribute in the window;
    assert((size_t)x >= m_left_boundary);
    m_text_attributes.emplace_back(y, x - m_left_boundary, length, attribute,
                                   color);
  }
};