#include <cctype>
#include <ncurses.h>

#include "TextBuffer.h"
#include "TextWindow.h"
#include "key_codes.h"

extern "C" {
void initialise() {
  initscr();
  start_color();
  noecho();
  // cbreak();
  raw();
  keypad(stdscr, TRUE);
}
}

void handle_key(TextBuffer &text_buffer, Key key) {
  if (key.is_insertable()) {
    text_buffer.insert_char_at(key.get_char());
    return;
  }

  if (key.is_modified()) {
    return;
  }

  if (key.is_type(KeyType::ENTER) && !key.is_modified()) {
    text_buffer.insert_line_at();
  }

  if (key.is_type(KeyType::BACKSPACE) && !key.is_modified()) {
    text_buffer.remove_char_at();
  }

  if (key.is_type(KeyType::ARROW)) {
    std::cerr << "it's an arrow key" << std::endl;
    if (key.has_keycode(UP)) {
      text_buffer.move_cursor_up();
    }
    if (key.has_keycode(DOWN)) {
      text_buffer.move_cursor_down();
    }
    if (key.has_keycode(LEFT)) {
      text_buffer.move_cursor_left();
    }
    if (key.has_keycode(RIGHT)) {
      text_buffer.move_cursor_right();
    }
  }
}

int main() {
  // construct an empty text buffer
  TextBuffer text_buffer;

  // prepares the standard screen; we should make stdscr explicit
  initialise();

  // get information about the screen
  int num_rows;
  int num_cols;
  getmaxyx(stdscr, num_rows, num_cols);

  // construct the main window and start on the top left corner
  TextWindow text_window(stdscr, num_rows, num_cols, 0, 0);

  // main event loop
  while (true) {
    int input_char = wgetch(stdscr);
    std::optional<Key> opt_key = keycode_to_key(input_char);

    // we might choose to ignore the currently obtained keypress
    if (!opt_key.has_value()) {
      continue;
    }

    // example of capturing something; we should shift this logic somewhere else
    // eventually i think
    Key key = opt_key.value();
    if (key.is_type(KeyType::ALPHA) && key.is_modified_by(KeyModifier::CTRL) &&
        key.get_char() == 'Q') {
      break;
    }

    // handling the key normally
    handle_key(text_buffer, key);

    // the logic here should be to obtain the string in full
    // then tag the string with the correct colours,
    // then update the screen
    wclear(stdscr);
    waddstr(stdscr, text_buffer.get_string().data());
    wrefresh(stdscr);
  }

  endwin(); // here's how you finish up ncurses mode
  // delwin(stdscr);

  return 0;
}