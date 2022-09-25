#include <ncurses.h>

#include "TextWindow.h"

// Serves as the driver for the entire view. For now let's keep it at a simple
//  thing that just holds a text_window, and given the state that needs to be
//  rendered drives the entire rendering logic
struct View {
  TextWindow m_text_window;

private:
  View(WINDOW *main_window_ptr, int height, int width)
      : text_window(main_window_ptr, height, width) {
  }

public:
  static View initialize() {
    initscr();
    start_color();
    noecho();
    raw();
    keypad(stdscr, TRUE);
    // get the screen height and width
    int height;
    int width;
    getmaxyx(stdscr, height, width);
    // construct the view with the main screen, and passing in height and width
    return View(stdscr, height, width);
  }

  // calls render on the relevant view elements, for now it's just a text view
  void render() {
    m_text_window.render();
  }

  // updates the required view elements?
  // this should take in things agnostic to how view works, and view does the
  // driving so view to should take a const ref to the Model and have free reign
  // on how it reads? create a model_view that only has view access to the model
  void update_view_state() {
  }
};