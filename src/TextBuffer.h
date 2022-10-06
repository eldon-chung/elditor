#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Cursor.h"
#include "Text.h"

// A class that holds the text for the text editor
class TextBuffer {
  std::vector<std::string> m_text_buffer;

public:
  TextBuffer() {
    // Ensure the buffer has at least an empty line
    m_text_buffer.emplace_back(std::string(""));
  }

  // Updates the cursor as it moves up
  void move_cursor_up(CursorPoint &cursor_point) {
    assert(within_bounds(cursor_point));
    if (cursor_point.row() > 0) {
      cursor_point.row()--;
      cursor_point.col() = std::min(cursor_point.original_col(), m_text_buffer.at(cursor_point.row()).size());
    } else {
      assert(cursor_point.row() == 0);
      cursor_point.col() = 0;
    }
    assert(within_bounds(cursor_point));
  }

  // Updates the cursor as it moves down
  void move_cursor_down(CursorPoint &cursor_point) {
    assert(within_bounds(cursor_point));
    if (cursor_point.row() + 1 < m_text_buffer.size()) {
      cursor_point.row()++;
      cursor_point.col() = std::min(m_text_buffer.at(cursor_point.row()).size(), cursor_point.original_col());
    } else {
      assert(cursor_point.row() + 1 == m_text_buffer.size());
      cursor_point.col() = m_text_buffer.at(cursor_point.row()).size();
    }
    assert(within_bounds(cursor_point));
  }

  // Updates the cursor as it moves left
  void move_cursor_left(CursorPoint &cursor_point) {
    assert(within_bounds(cursor_point));
    if (cursor_point.col() > 0) {
      cursor_point.col()--;
    } else if (cursor_point.row() > 0) {
      assert(cursor_point.col() == 0);
      // we move it to the previous line if needed
      cursor_point.row()--;
      cursor_point.col() = m_text_buffer.at(cursor_point.row()).size();
    }

    cursor_point.original_col() = cursor_point.col();
    assert(within_bounds(cursor_point));
  }

  // Updates the cursor as it moves right
  void move_cursor_right(CursorPoint &cursor_point) {
    assert(within_bounds(cursor_point));
    if (cursor_point.col() + 1 <= m_text_buffer.at(cursor_point.row()).length()) {
      cursor_point.col()++;
    } else if (cursor_point.col() == m_text_buffer.at(cursor_point.row()).length() &&
               cursor_point.row() + 1 < m_text_buffer.size()) {
      cursor_point.row()++;
      cursor_point.col() = 0;
    }

    cursor_point.original_col() = cursor_point.col();
    assert(within_bounds(cursor_point));
  }

  // Inserts to_insert at position specified by cursor
  void insert_string_at(std::string &&to_insert, Cursor &cursor) {
    // std::cerr << "inserting string: [" << to_insert << "] at cursor: [" << cursor << "]" << std::endl;

    // no text must be in selection when inserting text
    assert(!cursor.in_selection_mode());
    assert(within_bounds(cursor.active_point()));

    std::vector<std::string> broken_lines = break_into_lines(to_insert);
    assert(!broken_lines.empty());

    if (broken_lines.size() == 1) {
      // if there is only a single line then it should just hold the entirely of to_insert
      assert(to_insert == broken_lines.at(0));
      size_t to_insert_len = to_insert.size();
      // push the entire line into to the current line
      m_text_buffer.at(cursor.row()).insert(cursor.col(), std::move(to_insert));
      // update the cursor
      cursor.col() += to_insert_len;
    } else {
      // store the size the column needs to be for later
      size_t final_column = broken_lines.back().size();

      // get the second half of the line at row(), it needs to be prepended to the last of broken_lines
      std::string_view second_half_of_current_line{m_text_buffer.at(cursor.row()).cbegin() + cursor.col(),
                                                   m_text_buffer.at(cursor.row()).cend()};
      broken_lines.back().insert(0, second_half_of_current_line);

      // truncate the current line at row()
      m_text_buffer.at(cursor.row()).resize(cursor.col());

      // append the first of the broken_lines
      m_text_buffer.at(cursor.row()).append(std::move(broken_lines.at(0)));
      // insert the rest of the broken_lines (excluding the first)
      m_text_buffer.insert(m_text_buffer.cbegin() + cursor.row() + 1, broken_lines.begin() + 1,
                           broken_lines.end());

      cursor.row() += broken_lines.size() - 1;
      cursor.col() = final_column;
    }

    // update the cursor
    cursor.reset_original_col();
    cursor.reset_trailing_point();
    assert(within_bounds(cursor.active_point()));
  }

  // Removes text at position specified by cursor
  void remove_string_at(Cursor &cursor) {
    assert(within_bounds(cursor.active_point()));
    assert(within_bounds(cursor.trailing_point()));
    if (cursor.in_selection_mode()) {
      remove_with_selection_mode(cursor);
    } else {
      assert(!cursor.in_selection_mode());
      remove_without_selection_mode(cursor);
    }
    assert(within_bounds(cursor.active_point()));
    assert(within_bounds(cursor.trailing_point()));
  }

private:
  void remove_without_selection_mode(Cursor &cursor) {
    assert(!cursor.in_selection_mode());
    assert(within_bounds(cursor.active_point()));

    if (cursor.col() == 0 && cursor.row() > 0) {
      // get the current string, append it to the previous string,
      // update the cursor's column in the meantime
      cursor.col() = m_text_buffer.at(cursor.row() - 1).size();

      std::string current_row = std::move(m_text_buffer.at(cursor.row()));
      m_text_buffer.at(cursor.row() - 1).append(std::move(current_row));
      m_text_buffer.erase(m_text_buffer.begin() + cursor.row());
      cursor.row()--;
    } else if (cursor.col() > 0) {
      m_text_buffer.at(cursor.row()).erase(cursor.col() - 1, 1);
      cursor.col()--;
    }
    cursor.reset_original_col();
    cursor.reset_trailing_point();

    assert(!cursor.in_selection_mode());
    assert(within_bounds(cursor.active_point()));
  }

  void remove_with_selection_mode(Cursor &cursor) {
    assert(cursor.in_selection_mode());
    assert(within_bounds(cursor.active_point()));
    assert(within_bounds(cursor.trailing_point()));

    std::pair<CursorPoint &, CursorPoint &> c_point_pair = cursor.get_points_in_order();
    CursorPoint &left_point = c_point_pair.first;
    CursorPoint &right_point = c_point_pair.second;

    assert(left_point.row() < right_point.row() ||
           ((left_point.row() == right_point.row()) && left_point.col() < right_point.col()));

    if (left_point.row() == right_point.row()) {
      assert(left_point.col() < right_point.col());

      // erase the portion of the string that's in between
      auto line_it = m_text_buffer.at(left_point.row()).cbegin();
      m_text_buffer.at(left_point.row()).erase(line_it + left_point.col(), line_it + right_point.col());

    } else {
      assert(left_point.row() < right_point.row());
      assert(right_point.row() < m_text_buffer.size());

      // truncate the highest row
      m_text_buffer.at(left_point.row()).resize(left_point.col());

      // get the tail end of the last line
      std::string_view tail_end{m_text_buffer.at(right_point.row()).begin() + right_point.col(),
                                m_text_buffer.at(right_point.row()).end()};

      // append the tail end to the highest row
      m_text_buffer.at(left_point.row()).append(tail_end);

      // remove all the lines in after this one (including the row that right_point is sitting on)
      auto row_it = m_text_buffer.cbegin();
      m_text_buffer.erase(row_it + left_point.row() + 1, row_it + right_point.row() + 1);
    }

    // update the cursor by bringing it back to the left_point
    cursor.reset_to_point(left_point);

    assert(!cursor.in_selection_mode());
    assert(within_bounds(cursor.active_point()));
    assert(within_bounds(cursor.trailing_point()));
  }

public:
  // Returns the entire contents of the text buffer as a single string
  std::string get_as_string() const {
    std::string view_string{""};
    for (std::string const &line : m_text_buffer) {
      view_string += line;
      view_string += "\n";
    }
    return view_string;
  }

  std::string get_line_as_string(size_t line_idx) const {
    return m_text_buffer.at(line_idx);
  }

  Text get_text() const {
    std::string view_string{""};
    std::vector<size_t> line_lengths;
    line_lengths.reserve(m_text_buffer.size());

    for (size_t idx = 0; idx < m_text_buffer.size(); idx++) {
      view_string += m_text_buffer.at(idx);
      view_string += "\n";
      line_lengths.push_back(m_text_buffer.at(idx).size());
    }
    // remove the last "\n"
    view_string.pop_back();
    return Text{std::move(view_string), std::move(line_lengths)};
  }

private:
  std::vector<std::string_view> get_line_views(std::string const &str) {
    assert(!str.empty());
    std::string_view str_view{str};
    std::vector<std::string_view> line_views;
    do {
      size_t newl_idx = str_view.find_first_of("\n");
      if (newl_idx == std::string::npos) {
        line_views.push_back(str_view);
        break;
      }
      line_views.push_back(std::string_view{str_view.data(), newl_idx});
      str_view.remove_prefix(newl_idx + 1);
      if (str_view.empty()) {
        // if it's empty right after removing, it ended on a newline
        // so we need to append one more string
        line_views.push_back("");
      }
    } while (!str_view.empty());
    return line_views;
  }

  // to be fair since construction requires copying there might not be a
  // real need to use moves here
  std::vector<std::string> break_into_lines(std::string const &str) {
    assert(!str.empty());
    std::string_view str_view{str};
    std::vector<std::string> lines;
    do {
      size_t newl_idx = str_view.find_first_of("\n");
      if (newl_idx == std::string::npos) {
        lines.push_back(std::string{str_view});
        break;
      }
      lines.push_back(std::string{std::string_view{str_view.data(), newl_idx}});
      str_view.remove_prefix(newl_idx + 1);
      if (str_view.empty()) {
        // if it's empty right after removing, it ended on a newline
        // so we need to append one more string
        lines.push_back("");
      }
    } while (!str_view.empty());
    return lines;
  }

  bool within_bounds(CursorPoint const &cursor_point) const {
    return (cursor_point.row() >= 0 && cursor_point.row() < m_text_buffer.size()) &&
           (cursor_point.col() >= 0 && cursor_point.col() <= m_text_buffer.at(cursor_point.row()).size());
  }
};