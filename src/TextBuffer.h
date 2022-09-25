
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

  std::vector<std::string> text_buffer;
  Cursor m_cursor;

  // subsequently make one that is initialised with contents
  TextBuffer() : m_cursor(0, 0, 0) {
    // make an initial vector
    text_buffer.emplace_back(std::string(""));
  }

  // manipulate the cursor
  void move_cursor_up(Cursor &model_cursor) {
    if (m_cursor.row() > 0) {
      m_cursor.row()--;
      // update the column with a simple method for now
      m_cursor.col() =
          std::min(text_buffer[m_cursor.row()].length(), m_cursor.col());
    }

    // update the model_cursor as-is (this logic changes when we swap up the
    // cursor)
    model_cursor = m_cursor;
  }

  void move_cursor_down(Cursor &model_cursor) {
    if (m_cursor.row() + 1 < text_buffer.size()) {
      m_cursor.row()++;
      m_cursor.col() =
          std::min(text_buffer[m_cursor.row()].length(), m_cursor.col());
    }
    // update the model_cursor as-is (this logic changes when we swap up the
    // cursor)
    model_cursor = m_cursor;
  }

  void move_cursor_left(Cursor &model_cursor) {
    if (m_cursor.col() > 0) {
      m_cursor.col()--;
    }
    // update the model_cursor as-is (this logic changes when we swap up the
    // cursor)
    model_cursor = m_cursor;
  }

  void move_cursor_right(Cursor &model_cursor) {
    m_cursor.col()++;
    m_cursor.col() =
        std::min(text_buffer[m_cursor.row()].length(), m_cursor.col());
    // update the model_cursor as-is (this logic changes when we swap up the
    // cursor)
    model_cursor = m_cursor;
  }

  // insertion requires some form of specification
  // but for now let's just make it append
  void insert_char_at(char to_insert, Cursor &model_cursor) {
    text_buffer[m_cursor.row()].insert(m_cursor.col(), 1, to_insert);
    m_cursor.col()++;
    // update the model_cursor as-is (this logic changes when we swap up the
    // cursor)
    model_cursor = m_cursor;
  }

  void remove_char_at(Cursor &model_cursor) {
    // annoying things when you need to merge lines again
    if (m_cursor.col() == 0 && m_cursor.row() > 0) {
      // get the current string, append it to the previous string,
      std::string current_row = std::move(text_buffer[m_cursor.row()]);
      text_buffer[m_cursor.row() - 1].append(current_row);
      text_buffer.erase(text_buffer.begin() + m_cursor.row());
      // update the cursor
      move_cursor_up(model_cursor);
      m_cursor.col() = text_buffer[m_cursor.row()].length();
    } else if (m_cursor.col() > 0) {
      text_buffer[m_cursor.row()].erase(m_cursor.col() - 1, 1);
      move_cursor_left(model_cursor);
    }
  }

  void insert_line_at(Cursor &model_cursor) {
    // split the string at the current pos.
    // annoying things about being at the end of line
    if (m_cursor.col() == text_buffer[m_cursor.row()].length()) {
      text_buffer.insert(text_buffer.begin() + m_cursor.row() + 1,
                         std::string(""));
    }
    m_cursor.col() = 0;
    m_cursor.row()++;
    model_cursor = m_cursor;
  }

  // should you want string or string_view?
  std::string get_string() const {
    std::string view_string{""};
    for (std::string const &line : text_buffer) {
      view_string += line;
      view_string += "\n";
    }
    return view_string;
  }

  Text get_text() const {
    std::string view_string{""};
    std::vector<size_t> line_lengths;
    line_lengths.reserve(text_buffer.size());

    for (size_t idx = 0; idx < text_buffer.size(); idx++) {
      view_string += text_buffer.at(idx);
      view_string += "\n";
      line_lengths.push_back(text_buffer.at(idx).size());
    }
    // remove the last "\n"
    view_string.pop_back();
    {
      std::cerr << "TextBuffer : returning view string:" << view_string
                << std::endl;
      std::cerr << "TextBuffer : returning line_lengths: [";
      for (size_t length : line_lengths) {
        std::cerr << length << ", ";
      }
      std::cerr << "]" << std::endl;
    }
    return Text{std::move(view_string), std::move(line_lengths)};
  }

  Cursor get_cursor() const {
    return m_cursor;
  }
};