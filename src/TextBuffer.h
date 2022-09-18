#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Cursor.h"
// a basic text buffer that we can change later
// follow up question: should we make a "cursor aware" buffer?
// or perhaps a "buffer aware" cursor?
struct TextBuffer {

  std::vector<std::string> text_buffer;
  Cursor m_cursor;

  // subsequently make one that is initialised with contents
  TextBuffer() : m_cursor(0, 0) {
    // make an initial vector
    text_buffer.emplace_back(std::string(""));
  }

  // manipulate the cursor
  void move_cursor_up() {
    if (m_cursor.m_row > 0) {
      m_cursor.m_row--;
      // update the column with a simple method for now
      m_cursor.m_col =
          std::min(text_buffer[m_cursor.m_row].length(), m_cursor.m_col);
    }
  }

  void move_cursor_down() {
    if (m_cursor.m_row + 1 < text_buffer.size()) {
      m_cursor.m_row++;
      m_cursor.m_col =
          std::min(text_buffer[m_cursor.m_row].length(), m_cursor.m_col);
    }
  }

  void move_cursor_left() {
    if (m_cursor.m_col > 0) {
      std::cerr << "moved left once" << std::endl;
      m_cursor.m_col--;
    }
  }

  void move_cursor_right() {
    m_cursor.m_col++;
    m_cursor.m_col =
        std::min(text_buffer[m_cursor.m_row].length(), m_cursor.m_col);
  }

  // insertion requires some form of specification
  // but for now let's just make it append
  void insert_char_at(char to_insert) {
    text_buffer[m_cursor.m_row].insert(m_cursor.m_col, 1, to_insert);
    m_cursor.m_col++;
  }

  void remove_char_at() {
    // annoying things when you need to merge lines again
    if (m_cursor.m_col == 0 && m_cursor.m_row > 0) {
      // get the current string, append it to the previous string,
      std::string current_row = std::move(text_buffer[m_cursor.m_row]);
      text_buffer[m_cursor.m_row - 1].append(current_row);
      text_buffer.erase(text_buffer.begin() + m_cursor.m_row);
      // update the cursor
      move_cursor_up();
      m_cursor.m_col = text_buffer[m_cursor.m_row].length();
    } else if (m_cursor.m_col > 0) {
      text_buffer[m_cursor.m_row].erase(m_cursor.m_col - 1, 1);
      move_cursor_left();
    }
  }

  void insert_line_at() {
    // split the string at the current pos.
    // annoying things about being at the end of line
    if (m_cursor.m_col == text_buffer[m_cursor.m_row].length()) {
      text_buffer.insert(text_buffer.begin() + m_cursor.m_row + 1,
                         std::string(""));
    }
    m_cursor.m_col = 0;
    m_cursor.m_row++;
  }

  // should you want string or string_view?
  std::string get_string() {
    std::string view_string{""};
    for (std::string &line : text_buffer) {
      view_string += line;
      view_string += "\n";
    }
    return view_string;
  }
};