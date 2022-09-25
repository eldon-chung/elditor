#include <cassert>
#include <deque>
#include <ncurses.h>
#include <string>
#include <vector>

#include "Cursor.h"

struct TextWindow {
  WINDOW *m_window_ptr;
  std::deque<std::string> m_lines;
  // height of the screen
  size_t m_num_rows;
  // width of the screen
  size_t m_num_cols;
  // currently unused
  size_t m_upper_boundary;
  // which column of text to start the render
  size_t m_left_boundary;

  // if the boundaries are not defined, we delegate its construction
  TextWindow(WINDOW *window_ptr, size_t num_rows, size_t num_cols)
      : TextWindow(window_ptr, num_rows, num_cols, 0, 0) {
  }

  TextWindow(WINDOW *window_ptr, size_t num_rows, size_t num_cols,
             size_t upper_boundary, size_t left_boundary)
      : m_window_ptr(window_ptr), m_num_rows(num_rows), m_num_cols(num_cols),
        m_upper_boundary(upper_boundary), m_left_boundary(left_boundary) {
    for (size_t row = 0; row < m_num_rows; row++) {
      m_lines.push_back(std::move(std::string("")));
    }
  }

  void update(std::vector<std::string> &new_contents) {
    assert(new_contents.size() == m_num_rows);
    m_lines.clear();
    for (std::string &s : new_contents) {
      m_lines.push_back(std::move(s));
    }
    new_contents.clear();
  }

  void set_left_boundary(size_t left_boundary) {
    m_left_boundary = m_left_boundary;
  }

  void render() {
    // clear the current screen
    wclear(m_window_ptr);
    // get a "string_view" for each row and place it onto the screen
    for (size_t row_idx = 0; row_idx < m_num_rows; row_idx++) {
      if (m_left_boundary >= m_lines.at(row_idx).size()) {
        // if the starting column is too far right then we dont have to bother
        mvwaddstr(m_window_ptr, row_idx, 0, "");
      } else {
        // else there get a view of the current string, starting from start_col
        char *start_ptr = m_lines.at(row_idx).data() + m_left_boundary;
        std::string_view rendered_line{start_ptr, m_num_cols};
        mvwaddstr(m_window_ptr, row_idx, 0, rendered_line.data());
      }
    }

    wrefresh(m_window_ptr);
  }
};