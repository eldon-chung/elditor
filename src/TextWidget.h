#pragma once

#include <algorithm>
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
    std::vector<TaggedText> m_lines;
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
        : m_window_ptr(window_ptr), m_left_boundary(left_boundary), m_num_rows(num_rows),
          m_num_cols(num_cols) {
        for (size_t row = 0; row < m_num_rows; row++) {
            m_lines.push_back(std::string(""));
        }
    }

    void update(std::vector<TaggedText> &&new_contents) {
        assert(new_contents.size() == m_num_rows);
        m_lines.clear();
        m_lines = std::move(new_contents);
    }

    void render() {
        // should this be shifted into update?
        // then render just calls the rendering stuff;
        assert(m_lines.size() == m_num_rows);
        // clear the current screen
        werase(m_window_ptr);

        // get a "string_view" for each row and place it onto the screen
        for (size_t row_idx = 0; row_idx < m_num_rows; row_idx++) {
            mvwaddstr(m_window_ptr, row_idx, 0, m_lines.at(row_idx).get_text().data());
        }

        // place the attributes on the screen
        wstandend(m_window_ptr);
        for (size_t row_idx = 0; row_idx < m_num_rows; row_idx++) {
            for (TextTag const &tag : m_lines.at(row_idx).get_tags()) {
                mvwchgat(m_window_ptr, row_idx, tag.m_start_pos, tag.length(), (short)tag.m_attribute,
                         (short)tag.m_colour, NULL);
            }
        }

        wrefresh(m_window_ptr);
    }

    size_t height() const {
        return m_num_rows;
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
             row_idx < (size_t)m_text_window_border.ending_row() && row_idx < lines_in_window.size();
             ++row_idx) {
            lines_in_window.push_back(m_view_model->get_const_tagged_line_at(row_idx));
        }
        // pad it so that we have the correct amount
        while (lines_in_window.size() < m_text_window.height()) {
            lines_in_window.push_back(TaggedText{});
        }

        // cut out the text based on the box
        auto cut_out_line = [](TaggedText &line_to_cut, WindowBorder const &window_border) {
            // cut the line off the text if needed; implemented in an absolutely disgusting way
            std::string_view &line_view = line_to_cut.get_text();
            if (line_view.size() >= (size_t)window_border.starting_row()) {
                line_view.remove_prefix(window_border.starting_row());
                line_to_cut.resize(window_border.width());
            } else {
                line_view = "";
            }

            // now we need to cut the tags too
            for (TextTag &tag : line_to_cut.get_tags()) {
                if (tag.m_start_pos > (size_t)window_border.ending_col() ||
                    tag.m_end_pos < (size_t)window_border.starting_col()) {
                    // if the tag is either too far left or too far right; just 'delete' it
                    tag.m_end_pos = tag.m_start_pos;
                } else {
                    assert(tag.m_start_pos <= (size_t)window_border.ending_col() &&
                           tag.m_end_pos >= (size_t)window_border.starting_col());

                    // shrink them down so that they're within the range of the cut if necessary
                    tag.m_start_pos = std::max(tag.m_start_pos, (size_t)window_border.starting_col());
                    tag.m_end_pos = std::min(tag.m_end_pos, (size_t)window_border.ending_col());

                    // shift them left
                    tag.m_start_pos -= window_border.starting_col();
                    tag.m_end_pos -= window_border.starting_col();
                }
            }
        };

        for (TaggedText &line : lines_in_window) {
            cut_out_line(line, m_text_window_border);
        }

        // move the altered text into the text window
        m_text_window.update(std::move(lines_in_window));
    }
};