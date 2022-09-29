
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#pragma once

#include "Cursor.h"
#include "Text.h"
// a basic text buffer that we can change later
// follow up question: should we make a "cursor aware" buffer?
// or perhaps a "buffer aware" cursor?
struct TextBuffer {

  std::vector<std::string> m_text_buffer;
  Cursor m_cursor;

  // subsequently make one that is initialised with contents
  TextBuffer() : m_cursor(0, 0, 0) {
    // make an initial vector
    m_text_buffer.emplace_back(std::string(""));
  }

  // manipulate the cursor
  void move_cursor_up(Cursor &model_cursor) {
    if (m_cursor.row() > 0) {
      m_cursor.row()--;
      // update the column with a simple method for now
      m_cursor.col() = std::min(m_cursor.original_col(),
                                m_text_buffer.at(m_cursor.row()).size());
    }

    // update the model_cursor as-is (this logic changes when we swap up the
    // cursor)
    model_cursor = m_cursor;
  }

  void move_cursor_down(Cursor &model_cursor) {
    if (m_cursor.row() + 1 < m_text_buffer.size()) {
      m_cursor.row()++;
      m_cursor.col() = std::min(m_text_buffer[m_cursor.row()].length(),
                                m_cursor.original_col());
    }
    // update the model_cursor as-is (this logic changes when we swap up the
    // cursor)
    model_cursor = m_cursor;
  }

  void move_cursor_left(Cursor &model_cursor) {
    std::cerr << "TB: starting move left" << std::endl;
    if (m_cursor.col() > 0) {
      std::cerr << "TB: move left first case" << std::endl;

      m_cursor.col()--;
    } else if (m_cursor.row() > 0) {
      assert(m_cursor.col() == 0);
      // we move it to the previous line if needed
      std::cerr << "TB: move left second case" << std::endl;

      m_cursor.row()--;
      m_cursor.col() = m_text_buffer.at(m_cursor.row()).size();
    }

    m_cursor.original_col() = m_cursor.col();
    std::cerr << "TB: after moving left: row col ori:" << m_cursor.row() << " "
              << m_cursor.col() << " " << m_cursor.original_col() << std::endl;
    model_cursor = m_cursor;
  }

  void move_cursor_right(Cursor &model_cursor) {
    std::cerr << "TB: starting move right from " << m_cursor.row() << " "
              << m_cursor.col() << std::endl;
    std::cerr << "TB: line length is " << m_text_buffer[m_cursor.row()].length()
              << std::endl;
    if (m_cursor.col() + 1 <= m_text_buffer[m_cursor.row()].length()) {
      std::cerr << "TB: move right first case" << std::endl;
      m_cursor.col()++;
    } else if (m_cursor.col() == m_text_buffer[m_cursor.row()].length() &&
               m_cursor.row() + 1 < m_text_buffer.size()) {
      std::cerr << "TB: move right second case" << std::endl;

      m_cursor.row()++;
      m_cursor.col() = 0;
    }

    m_cursor.original_col() = m_cursor.col();
    std::cerr << "TB: after moving right: row col ori:" << m_cursor.row() << " "
              << m_cursor.col() << " " << m_cursor.original_col() << std::endl;
    model_cursor = m_cursor;
  }

  // insertion requires some form of specification
  // but for now let's just make it append
  void insert_char_at(char to_insert, Cursor &model_cursor) {
    m_text_buffer[m_cursor.row()].insert(m_cursor.col(), 1, to_insert);
    m_cursor.col()++;
    m_cursor.original_col() = m_cursor.col();
    // update the model_cursor as-is (this logic changes when we swap up the
    // cursor)
    model_cursor = m_cursor;
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
      m_cursor.row()--;

    } else if (m_cursor.col() > 0) {
      m_text_buffer[m_cursor.row()].erase(m_cursor.col() - 1, 1);
      m_cursor.col()--;
    }
    m_cursor.original_col() = m_cursor.col();

    model_cursor = m_cursor;
  }

  void insert_line_at(Cursor &model_cursor) {
    // split the string at the current pos.
    // annoying things about being at the end of line
    if (m_cursor.col() == m_text_buffer[m_cursor.row()].length()) {
      m_text_buffer.insert(m_text_buffer.begin() + m_cursor.row() + 1,
                           std::string(""));
    } else if (m_cursor.col() == 0) {
      m_text_buffer.insert(m_text_buffer.begin() + m_cursor.row(),
                           std::string(""));
    } else {
      // get the second half of the current string
      std::string second_half =
          m_text_buffer.at(m_cursor.row()).substr(m_cursor.col());
      // truncate the original string to the first half
      m_text_buffer.at(m_cursor.row()).resize(m_cursor.col());

      m_text_buffer.insert(m_text_buffer.begin() + m_cursor.row() + 1,
                           std::move(second_half));
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

  Cursor get_cursor() const {
    return m_cursor;
  }
};