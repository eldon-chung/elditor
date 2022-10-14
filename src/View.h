#pragma once
#include <cassert>
#include <curses.h>
#include <ncurses.h>
#include <utility>

#include "Colours.h"
#include "Model.h"
#include "Text.h"
#include "TextWidget.h"
#include "ViewModel.h"

// Serves as the driver for the entire view. For now let's keep it at a simple
//  thing that just holds a text_window, and given the state that needs to be
//  rendered drives the entire rendering logic
class View {
  ViewModel const *m_view_model;
  TextWidget m_text_widget;

private:
  View(ViewModel const *view_model, WINDOW *main_window_ptr, int height, int width)
      : m_view_model(view_model), m_text_widget(view_model, main_window_ptr, height, width) {
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

    // Get the screen height and width
    int height, width;
    getmaxyx(stdscr, height, width);
    std::cerr << "View: initialised with height and width " << height << " " << width << std::endl;
    // Construct the view with the main screen, and passing in height and width
    return View(model, stdscr, height, width);
  }

  // Calls render on the relevant view elements
  void render() {
    m_text_widget.render();
  }

  void update_state() {
    m_text_widget.update_state();
  }
};
