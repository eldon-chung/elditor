#pragma once
#include "Text.h"
#include "TextBuffer.h"
#include "file.h"

// Conceptually stores the state of the program
class Model {
    Cursor m_cursor;
    FileHandle m_file_handle;
    TextBuffer m_text_buffer;

    Model() : m_cursor{0, 0, 0} {
    }

    Model(std::string pathname)
        : m_cursor{0, 0, 0}, m_file_handle(std::move(pathname)), m_text_buffer(m_file_handle.read()) {
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

    // do we even need this
    static Model initialise(std::string pathname) {
        return Model(std::move(pathname));
    }

    void open_file(std::string pathname) {
        // if our file handle has a file right now
        if (m_file_handle.is_handle_to_file()) {
            // write out the contents
            m_file_handle.save(m_text_buffer.get_as_string());
        }

        // open the new file
        m_file_handle.open(std::move(pathname));
        m_text_buffer = TextBuffer(m_file_handle.read());
    }

    void save_to_file() {
        // if our file handle has a file right now
        if (m_file_handle.is_handle_to_file()) {
            // write out the contents
            // std::cerr << "Model: Save was called!" << std::endl;
            m_file_handle.save(m_text_buffer.get_as_string());
        } else {
            // we should prompt the user for a name instead of simply returning
            std::cerr << "Model: No file open to save to right now." << std::endl;
            return;
        }
    }

    void insert_string(std::string &&to_insert) {
        if (m_cursor.in_selection_mode()) {
            m_text_buffer.remove_string_at(m_cursor);
        }
        m_text_buffer.insert_string_at(std::move(to_insert), m_cursor);
    }

    void remove_char() {
        m_text_buffer.remove_string_at(m_cursor);
    }

    // Base cursor movement

    void move_cursor_up() {
        m_text_buffer.move_cursor_up(m_cursor.active_point());
        m_cursor.reset_trailing_point();
    }

    void move_cursor_down() {
        m_text_buffer.move_cursor_down(m_cursor.active_point());
        m_cursor.reset_trailing_point();
    }

    void move_cursor_left() {
        if (m_cursor.in_selection_mode()) {
            CursorPoint left_point = m_cursor.get_left_point();
            m_cursor.reset_to_point(left_point);
        } else {
            m_text_buffer.move_cursor_left(m_cursor.active_point());
            m_cursor.reset_trailing_point();
        }
    }

    void move_cursor_right() {
        if (m_cursor.in_selection_mode()) {
            CursorPoint right_point = m_cursor.get_right_point();
            m_cursor.reset_to_point(right_point);
        } else {
            m_text_buffer.move_cursor_right(m_cursor.active_point());
            m_cursor.reset_trailing_point();
        }
    }

    // Shift cursor movement

    void shift_cursor_up() {
        m_text_buffer.move_cursor_up(m_cursor.active_point());
    }

    void shift_cursor_down() {
        m_text_buffer.move_cursor_down(m_cursor.active_point());
    }

    void shift_cursor_left() {
        m_text_buffer.move_cursor_left(m_cursor.active_point());
    }

    void shift_cursor_right() {
        m_text_buffer.move_cursor_right(m_cursor.active_point());
    }

    // const view api
    Text get_text() const {
        return m_text_buffer.get_text();
    }

    Cursor get_cursor() const {
        return m_cursor;
    }
};