#include <cctype>
#include <ncurses.h>

#include "Model.h"
#include "TextBuffer.h"
#include "View.h"
#include "file.h"
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

void handle_key(Model &model, Key key) {
    if (key.is_insertable()) {
        model.insert_string(std::string(1, key.get_char()));
        return;
    }

    // if (key.is_modified()) {
    //   return;
    // }

    if (key.is_type(KeyType::ENTER) && !key.is_modified()) {
        model.insert_string(std::string("\n"));
    }

    if (key.is_type(KeyType::BACKSPACE) && !key.is_modified()) {
        model.remove_char();
    }

    if (key.is_type(KeyType::ARROW)) {
        // std::cerr << "key modifier:[" << key.modifier() << "]" << std::endl;
        if (!key.is_modified()) {
            if (key.has_keycode(UP)) {
                model.move_cursor_up();
            }
            if (key.has_keycode(DOWN)) {
                model.move_cursor_down();
            }
            if (key.has_keycode(LEFT)) {
                model.move_cursor_left();
            }
            if (key.has_keycode(RIGHT)) {
                model.move_cursor_right();
            }
        } else if (key.is_modified_by(KeyModifier::SHIFT)) {
            if (key.has_keycode(SHIFT_UP)) {
                model.shift_cursor_up();
            }
            if (key.has_keycode(SHIFT_DOWN)) {
                model.shift_cursor_down();
            }
            if (key.has_keycode(SHIFT_LEFT)) {
                model.shift_cursor_left();
            }
            if (key.has_keycode(SHIFT_RIGHT)) {
                model.shift_cursor_right();
            }
        }
    }
}

int main(int argc, char **argv) {

    // construct model and give view a "handle" to model
    Model model = Model::initialize();
    if (argc > 1) {
        // if there is a filename (and ignore the subsequent arguments)
        model.open_file(std::string(argv[1]));
    }

    ViewModel view_model = ViewModel(&model);
    View view = View::initialize(&view_model);

    // main event loop
    while (true) {
        // update view model
        view_model.prepare_view_data();

        // get view to update its state
        view.update_state();
        view.render();

        // make this nonblocking
        int input_char = wgetch(stdscr);
        std::optional<Key> opt_key = keycode_to_key(input_char);

        // we might choose to ignore the currently obtained keypress
        if (!opt_key.has_value()) {
            continue;
        }

        // example of capturing something; we should shift this logic somewhere else
        // eventually i think
        Key key = opt_key.value();
        if (key.is_type(KeyType::ALPHA) && key.is_modified_by(KeyModifier::CTRL) && key.get_char() == 'Q') {
            break;
        }

        if (key.is_type(KeyType::ALPHA) && key.is_modified_by(KeyModifier::CTRL) && key.get_char() == 'S') {
            model.save_to_file();
        }

        // handling the key normally
        handle_key(model, key);

        // the logic here should be to obtain the string in full
        // then tag the string with the correct colours,
        // then update the screen
    }

    endwin(); // here's how you finish up ncurses mode
    // delwin(stdscr);

    return 0;
}