#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Cursor.h"
#include "Text.h"
// a basic text buffer that we can change later
// follow up question: should we make a "cursor aware" buffer?
// or perhaps a "buffer aware" cursor?
class TextBuffer {
  std::vector<std::string> m_text_buffer;

public:
  // subsequently make one that is initialised with contents
  TextBuffer() {
    // make an initial vector
    m_text_buffer.emplace_back(std::string(""));
  }

  // manipulate the cursor
  void move_cursor_up(CursorPoint &cursor_point) {
    if (cursor_point.row() > 0) {
      cursor_point.row()--;
      // update the column with a simple method for now
      cursor_point.col() = std::min(cursor_point.original_col(), m_text_buffer.at(cursor_point.row()).size());
    }
  }

  void move_cursor_down(CursorPoint &cursor_point) {
    if (cursor_point.row() + 1 < m_text_buffer.size()) {
      cursor_point.row()++;
      cursor_point.col() = std::min(m_text_buffer[cursor_point.row()].length(), cursor_point.original_col());
    }
  }

  void move_cursor_left(CursorPoint &cursor_point) {
    if (cursor_point.col() > 0) {
      cursor_point.col()--;
    } else if (cursor_point.row() > 0) {
      assert(cursor_point.col() == 0);
      // we move it to the previous line if needed
      cursor_point.row()--;
      cursor_point.col() = m_text_buffer.at(cursor_point.row()).size();
    }

    cursor_point.original_col() = cursor_point.col();
  }

  void move_cursor_right(CursorPoint &cursor_point) {
    if (cursor_point.col() + 1 <= m_text_buffer[cursor_point.row()].length()) {
      cursor_point.col()++;
    } else if (cursor_point.col() == m_text_buffer[cursor_point.row()].length() &&
               cursor_point.row() + 1 < m_text_buffer.size()) {
      cursor_point.row()++;
      cursor_point.col() = 0;
    }

    cursor_point.original_col() = cursor_point.col();
  }

  // insertion requires some form of specification
  // but for now let's just make it append
  void insert_char_at(std::string to_insert, Cursor &cursor) {
    // no text must be in selection when inserting text
    assert(!cursor.in_selection_mode());

    std::vector<std::string_view> broken_lines = break_lines(to_insert);
    assert(!broken_lines.empty());

    if (broken_lines.size() == 1) {
      // if there is only a single line then it should just hold the entirely of to_insert
      assert(to_insert == broken_lines.at(0));
      size_t to_insert_len = to_insert.size();
      // push the entire line into to the current line
      m_text_buffer[cursor.row()].insert(cursor.col(), std::move(to_insert));
      // update the cursor
      cursor.col() += to_insert_len;
      cursor.reset_original_col();
      return;
    }

    // construct the right half (what happens when you move from back; probably impl defined)
    std::string right_half{std::move(broken_lines.back())};
    broken_lines.pop_back();
    right_half.append(m_text_buffer[cursor.row()], cursor.col(), std::string::npos);

    // truncate down the left half
    m_text_buffer[cursor.row()].resize(cursor.col());
    m_text_buffer[cursor.row()].append(std::move(broken_lines.front()));

    // insert the middle portion

    // update the cursor
  }

  void remove_char_at(Cursor &model_cursor) {
    // annoying things when you need to merge lines again
    if (m_cursor.col() == 0 && m_cursor.row() > 0) {
      // get the current string, append it to the previous string,
      size_t col_length = m_text_buffer[m_cursor.row() - 1].size();

      std::string current_row = std::move(m_text_buffer[m_cursor.row()]);
      m_text_buffer[m_cursor.row() - 1].append(current_row);
      m_text_buffer.erase(m_text_buffer.begin() + m_cursor.row());

      // update the cursor
      m_cursor.col() = col_length;

    } else if (m_cursor.col() > 0) {
      m_text_buffer[m_cursor.row()].erase(m_cursor.col() - 1, 1);
      m_cursor.col()--;
    }
    m_cursor.original_col() = m_cursor.col();

    model_cursor = m_cursor;
  }

  void insert_line_at(Cursor &model_cursor) {
    // must inset without having anything selected
    assert(!model_cursor.in_selection_mode());

    // split the string at the current pos.
    // annoying things about being at the end of line

    if (m_cursor.col() == m_text_buffer[m_cursor.row()].length()) {
      m_text_buffer.insert(m_text_buffer.begin() + m_cursor.row() + 1, std::string(""));
    } else if (m_cursor.col() == 0) {
      m_text_buffer.insert(m_text_buffer.begin() + m_cursor.row(), std::string(""));
    } else {
      // get the second half of the current string
      std::string second_half = m_text_buffer.at(m_cursor.row()).substr(m_cursor.col());
      // truncate the original string to the first half
      m_text_buffer.at(m_cursor.row()).resize(m_cursor.col());

      m_text_buffer.insert(m_text_buffer.begin() + m_cursor.row() + 1, std::move(second_half));
    }
    m_cursor.col() = 0;
    m_cursor.original_col() = 0;
    m_cursor.row()++;
    model_cursor = m_cursor;
  }

  // should you want string or string_view?
  std::string get_string() const {
    std::string view_string{""};
    for (std::string const &line : m_text_buffer) {
      view_string += line;
      view_string += "\n";
    }
    return view_string;
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
    // {
    //   std::cerr << "TextBuffer : returning view string:" << view_string
    //             << std::endl;
    //   std::cerr << "TextBuffer : returning line_lengths: [";
    //   for (size_t length : line_lengths) {
    //     std::cerr << length << ", ";
    //   }
    //   std::cerr << "]" << std::endl;
    // }
    return Text{std::move(view_string), std::move(line_lengths)};
  }

private:
  std::vector<std::string_view> break_lines(std::string const &str) {
    assert(!str.empty());
    std::string_view str_view{str};
    std::vector<std::string_view> lines;
    do {
      size_t newl_idx = str_view.find_first_of("\n");
      if (newl_idx == std::string::npos) {
        lines.push_back(str_view);
        break;
      }
      lines.push_back(std::string_view{str_view.data(), newl_idx});
      str_view.remove_prefix(newl_idx + 1);
      if (str_view.empty()) {
        // if it's empty right after removing, it ended on a newline
        // so we need to append one more string
        lines.push_back("");
      }
    } while (!str_view.empty());
    return lines;
  }
};