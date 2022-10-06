#pragma once
#include "Text.h"
#include "TextBuffer.h"

// Conceptually stores the state of the program
class Model {
  TextBuffer m_text_buffer;
  Cursor m_cursor;

  Model() : m_cursor{0, 0, 0} {
  }

public:
  Model(Model const &) = delete;
  Model &operator=(Model const &) = delete;
  Model(Model &&) = delete;
  Model &operator=(Model &&) = delete;
  ~Model() {
  }

  static Model initialize() {
    return Model();
  }

  void insert_string(std::string &&to_insert) {
    m_text_buffer.insert_string_at(std::move(to_insert), m_cursor);
  }

  void remove_char() {
    m_text_buffer.remove_string_at(m_cursor);
  }

  void move_cursor_up() {
    m_text_buffer.move_cursor_up(m_cursor.active_point());
    m_cursor.reset_trailing_point();
  }

  void move_cursor_down() {
    m_text_buffer.move_cursor_down(m_cursor.active_point());
    m_cursor.reset_trailing_point();
  }

  void move_cursor_left() {
    m_text_buffer.move_cursor_left(m_cursor.active_point());
    m_cursor.reset_trailing_point();
  }

  void move_cursor_right() {
    m_text_buffer.move_cursor_right(m_cursor.active_point());
    m_cursor.reset_trailing_point();
  }

  // const view api
  Text get_text() const {
    return m_text_buffer.get_text();
  }

  Cursor get_cursor() const {
    return m_cursor;
  }
};