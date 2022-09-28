#pragma once
#include <cassert>
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
      : m_model(model), m_text_window(main_window_ptr, height, width),
        m_text_window_border(height, width) {
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
    std::cerr << "View: initialised with height and width " << height << " "
              << width << std::endl;
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

    for (size_t row_idx = m_text_window_border.starting_row();
         row_idx < (size_t)m_text_window_border.ending_row() &&
         row_idx < text.num_lines();
         row_idx++) {
      if ((size_t)m_text_window_border.starting_col() >=
          text.get_line_at(row_idx).size()) {
        lines_to_render.emplace_back(std::string_view{""});
      } else {
        lines_to_render.push_back(text.get_line_at(row_idx).substr(
            m_text_window_border.starting_col(),
            m_text_window_border.width() - 1));
      }
    }
    // pad it so that we have the correct amount
    while (lines_to_render.size() < m_text_window.height()) {
      lines_to_render.push_back(std::string_view(""));
    }

    m_text_window.update(std::move(lines_to_render));
    m_text_window.add_attribute(
        cursor.row() - m_text_window_border.starting_row(),
        cursor.col() - m_text_window_border.starting_col(), 1, A_STANDOUT, 0);
  }
};
