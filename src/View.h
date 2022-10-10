#pragma once
#include <cassert>
#include <curses.h>
#include <ncurses.h>
#include <utility>

#include "Colours.h"
#include "Model.h"
#include "Text.h"
#include "TextTag.h"
#include "TextWindow.h"
#include "WindowBorder.h"

// Serves as the driver for the entire view. For now let's keep it at a simple
//  thing that just holds a text_window, and given the state that needs to be
//  rendered drives the entire rendering logic
class View {
  Model const *m_model;
  TextWindow m_text_window;
  WindowBorder m_text_window_border;

private:
  View(Model *model, WINDOW *main_window_ptr, int height, int width)
      : m_model(model), m_text_window(main_window_ptr, height, width), m_text_window_border(height, width) {
  }

public:
  // important shit
  View(View const &) = delete;
  View &operator=(View const &) = delete;
  View(View &&) = delete;
  View &operator=(View &&) = delete;
  ~View() {
  }

  static void init_view_colours() {
    init_pair(COLOUR::NORMAL, COLOR_WHITE, -1);
    init_pair(COLOUR::CURSOR, COLOR_BLACK, COLOR_WHITE);
  }

  static View initialize(Model *model) {
    initscr();
    start_color();
    use_default_colors();
    noecho();
    raw();
    curs_set(0);
    keypad(stdscr, TRUE);

    // get the screen height and width
    int height, width;
    getmaxyx(stdscr, height, width);
    std::cerr << "View: initialised with height and width " << height << " " << width << std::endl;
    // construct the view with the main screen, and passing in height and width
    return View(model, stdscr, height, width);
  }

  // calls render on the relevant view elements, for now it's just a text view
  void render() {
    m_text_window.render();
  }

  void update_state() {
    // get relevant portions of the model
    Text text = m_model->get_text();
    Cursor cursor = m_model->get_cursor();

    // update the window to "chase the cursor"
    m_text_window_border.chase_point(cursor.row(), cursor.col());

    // now based on these we fetch the required parts of the strings
    std::vector<std::string_view> lines_to_render;
    lines_to_render.reserve(m_text_window.height());

    // std::cerr << "text window borders: starting row:[" << m_text_window_border.starting_row() << "]"
    //           << std::endl;
    // std::cerr << "text window borders: ending row:[" << m_text_window_border.ending_row() << "]" <<
    // std::endl; std::cerr << "text window borders: starting col:[" << m_text_window_border.starting_col() <<
    // "]"
    //           << std::endl;
    // std::cerr << "text window borders: ending col:[" << m_text_window_border.ending_col() << "]" <<
    // std::endl;

    for (size_t row_idx = m_text_window_border.starting_row();
         row_idx < (size_t)m_text_window_border.ending_row() && row_idx < text.num_lines(); ++row_idx) {
      if ((size_t)m_text_window_border.starting_col() >= text.get_line_at(row_idx).size()) {
        // std::cerr << "adding empty line" << std::endl;
        lines_to_render.emplace_back(std::string_view{""});
      } else {
        // std::cerr << "adding line:["
        //           << text.get_line_at(row_idx).substr(m_text_window_border.starting_col(),
        //                                               m_text_window_border.width() - 1)
        //           << "]" << std::endl;
        lines_to_render.push_back(text.get_line_at(row_idx).substr(m_text_window_border.starting_col(),
                                                                   m_text_window_border.width() - 1));
      }
    }

    // pad it so that we have the correct amount
    while (lines_to_render.size() < m_text_window.height()) {
      // std::cerr << "padding empty line" << std::endl;
      lines_to_render.push_back(std::string_view(""));
    }

    // update the contents
    m_text_window.update(std::move(lines_to_render));

    // special logic for rendering the cursor
    std::cerr << "rendering cursor in mode:[" << cursor.in_selection_mode() << std::endl;
    render_cursor(cursor);
  }

  void render_cursor(Cursor cursor) {
    // special logic for rendering the cursor; we need to deal with the case where we select more rows than
    // the cursor it needs to chase the active point
    if (!cursor.in_selection_mode()) {
      // if it is not in selection mode, just underline it
      m_text_window.add_attribute(cursor.row() - m_text_window_border.starting_row(),
                                  cursor.col() - m_text_window_border.starting_col(), 1, A_STANDOUT, 0);
      return;
    }
    std::cerr << "rendering non selection cursor " << std::endl;
    std::pair<CursorPoint, CursorPoint> point_pair = cursor.get_points_in_order();
    CursorPoint left_point = point_pair.first;
    CursorPoint right_point = point_pair.second;
    // else we make the relevant portions standout
    if (left_point.row() == right_point.row()) {
      assert(left_point.col() < right_point.col());
      m_text_window.add_attribute(left_point.row() - m_text_window_border.starting_row(), left_point.col(),
                                  right_point.col() - left_point.col(), A_UNDERLINE, 0);

      return;
    }
    // render the first line of the cursor
    // size_t first_line_length = m_text_window.get_line_length_at(0);
    //   m_text_window.add_attribute(left_point.row() - m_text_window_border.starting_row(),
    //                               left_point.col() - m_text_window_border.starting_col(),
    //                               first_line_length - left_point.col(), A_STANDOUT, 0);
    //   // render the middle lines of the cursor
    //   for (size_t line_idx = left_point.row() + 1; line_idx < right_point.row(); ++line_idx) {
    //     m_text_window.add_attribute(line_idx - m_text_window_border.starting_row(), left_point.col() )
    //   }
    //   // render the final lines of the cursor

    //   return;
    // }

    // assert(left_point.row() < right_point.row());
  }
};
