#include <cctype>
#include <ncurses.h>

#include "TextBuffer.h"
#include "key_codes.h"

extern "C" {
void initialise() {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
}
}

int main() {
  // construct an empty text buffer
  TextBuffer text_buffer;

  // prepares the standard screen; we should make stdscr explicit
  initialise();

  // main event loop
  while (true) {
    int input_char = wgetch(stdscr);
    // for now let's just handle alphabetic key chars;
    if (std::isalpha(input_char)) {
      // if char is alphabetic, we add it into a buffer
      text_buffer.insert_char_at(input_char);
      std::string buffer_contents = text_buffer.get_string();
      wclear(stdscr);
      waddstr(stdscr, buffer_contents.data());
      refresh();
    } else if (input_char == CONTROL_LEFT) {
      wclear(stdscr);
      waddstr(stdscr, "lctrl pressed");
      refresh();
    } else {
      // if it's anything else we break from the main event loop;
      break;
    }

    // print contents of buffer into the screen
    // ncurses printw expects a char* (we need to wrap around this too i guess)
  }

  delwin(stdscr);
  endwin(); // here's how you finish up ncurses mode

  return 0;
}