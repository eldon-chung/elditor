#include <bitset>
#include <ncurses.h>

#include "key_codes.h"

void initialise() {
  initscr();
  noecho();
  raw();
  keypad(stdscr, TRUE);
}

int main() {
  initialise();
  while (true) {
    int key_code = getch();
    std::optional<Key> key = keycode_to_key(key_code);
    if (key_code == 'X') {
      // just so we dont have to hit ctrl C
      break;
    }
    wclear(stdscr);
    const char *keyname_ptr = keyname(key_code);
    wprintw(stdscr, "keycode: %d keyname: %s\n", key_code, keyname_ptr);
    if (key.has_value()) {
      wprintw(stdscr, "key struct: %s\n", key.value().to_string().data());
    } else {
      wprintw(stdscr, "keycode ignored\n");
      wrefresh(stdscr);
    }
  }

  endwin();
  delwin(stdscr);
}