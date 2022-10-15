#pragma once

#include <cassert>
#include <deque>
#include <ncurses.h>
#include <string>
#include <vector>

#include "Cursor.h"
#include "Model.h"
#include "Text.h"
#include "TextAttribute.h"
#include "ViewModel.h"
#include "WindowBorder.h"

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

  TextWindow(WINDOW *window_ptr, size_t num_rows, size_t num_cols, size_t left_boundary)
      : m_window_ptr(window_ptr), m_left_boundary(left_boundary), m_num_rows(num_rows), m_num_cols(num_cols) {
    for (size_t row = 0; row < m_num_rows; row++) {
      m_lines.push_back(std::string(""));
    }
  }

  void update(std::vector<std::string_view> &&new_contents) {
    assert(new_contents.size() == m_num_rows);
    m_text_attributes.clear();
    m_lines.clear();
    for (std::string_view &s : new_contents) {
      m_lines.push_back(std::string(s));
    }
    new_contents.clear();
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
        mvwaddstr(m_window_ptr, row_idx, 0, "");
      } else {
        // else there get a view of the current string, starting from start_col
        const char *start_ptr = m_lines.at(row_idx).data() + m_left_boundary;
        std::string_view rendered_line{start_ptr, m_num_cols};
        mvwaddstr(m_window_ptr, row_idx, 0, rendered_line.data());
      }
    }

    // place the attributes on the screen
    wstandend(m_window_ptr);
    for (TextAttribute &text_attribute : m_text_attributes) {
      mvwchgat(m_window_ptr, text_attribute.y(), text_attribute.x(), text_attribute.n(),
               text_attribute.attribute(), text_attribute.colour(), NULL);
    }

    wrefresh(m_window_ptr);
  }

  size_t height() const {
    return m_num_rows;
  }

  void add_attribute(int y, int x, int length, attr_t attribute, short color) {
    // translate this into an attribute in the window;
    assert((size_t)x >= m_left_boundary);
    m_text_attributes.emplace_back(y, x - m_left_boundary, length, attribute, color);
  }

  size_t get_line_length_at(size_t index) const {
    return m_lines.at(index).size();
  }
};

// The driver class that drives the TextWindow class
class TextWidget {
  ViewModel const *m_view_model;
  TextWindow m_text_window;
  WindowBorder m_text_window_border;

public:
  TextWidget(ViewModel const *view_model, WINDOW *main_window_ptr, int height, int width)
      : m_view_model(view_model), m_text_window(main_window_ptr, height, width),
        m_text_window_border(height, width) {
  }

  ~TextWidget() {
  }
  TextWidget(TextWidget const &) = delete;
  TextWidget &operator=(TextWidget const &) = delete;
  TextWidget(TextWidget &&) = delete;
  TextWidget &operator=(TextWidget &&) = delete;

  void render() {
    m_text_window.render();
  }

  void update_state() {
    // get the cursor
    Cursor cursor = m_view_model->get_cursor();

    // update the window to "chase the cursor"
    m_text_window_border.chase_point(cursor.row(), cursor.col());

    // get the relevant strings within the rows of the current border
    std::vector<TaggedText> lines_in_window;
    lines_in_window.reserve(m_text_window_border.height());
    for (size_t row_idx = m_text_window_border.starting_row();
         row_idx < (size_t)m_text_window_border.ending_row() && row_idx < lines_in_window.size(); ++row_idx) {
      lines_in_window.push_back(m_view_model->get_const_tagged_line_at(row_idx));
    }
    // pad it so that we have the correct amount
    while (lines_in_window.size() < m_text_window.height()) {
      // std::cerr << "padding empty line" << std::endl;
      lines_in_window.push_back(TaggedText{});
    }

    // cut out the text based on the box

    // move the altered text into the text window
    // m_text_window.update(std::move(lines_in_window));
  }
};