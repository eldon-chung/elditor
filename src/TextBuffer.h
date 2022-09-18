#include <cstdint>
#include <string>
#include <vector>

#include "Cursor.h"
// a basic text buffer that we can change later
// follow up question: should we make a "cursor aware" buffer?
// or perhaps a "buffer aware" cursor?
struct TextBuffer {

  std::vector<std::string> text_buffer;
  std::size_t m_numlines;
  std::size_t m_numcols;
  Cursor m_cursor;

  // subsequently make one that is initialised with contents
  TextBuffer() : m_numlines(1), m_numcols(0), m_cursor(1, 0) {
    // make an initial vector
    text_buffer.emplace_back(std::string(""));
  }

  // manipulate the cursor
  void move_cursor_up() {
    if (m_cursor.m_row > 0) {
      m_cursor.m_row--;
    }
  }

  void move_cursor_down() {
    if (m_cursor.m_row < m_numlines) {
      m_cursor.m_row++;
    }
  }

  void move_cursor_left() {
    if (m_cursor.m_col > 0) {
      m_cursor.m_col--;
    }
  }

  void move_cursor_right() {
    if (m_cursor.m_col < m_numcols) {
      m_cursor.m_col++;
    }
  }

  // insertion requires some form of specification
  // but for now let's just make it append
  void insert_char_at(char to_insert) {
    text_buffer[0].push_back(to_insert);
  }

  // should you want string or string_view?
  std::string get_string() {
    return text_buffer.front();
  }
};