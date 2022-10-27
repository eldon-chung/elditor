#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "file.h"

std::vector<std::string_view> break_lines(std::string const &str) {
    assert(!str.empty());
    std::string_view str_view{str};
    std::vector<std::string_view> lines;
    do {
        size_t newl_idx = str_view.find_first_of("\n");
        if (newl_idx == std::string::npos) {
            lines.push_back(str_view);
            break;
        }
        lines.push_back(std::string_view{str_view.data(), newl_idx});
        str_view.remove_prefix(newl_idx + 1);
        if (str_view.empty()) {
            // if it's empty right after removing, it ended on a newline
            // so we need to append one more string
            lines.push_back("");
        }
    } while (!str_view.empty());
    return lines;
}

int main() {
    FileHandle fh("./sample_files/multiline.txt");
    std::string file_contents = fh.read();

    std::cout << "[" << file_contents << "]" << std::endl;

    std::string new_contents{"{}"};
    fh.save(new_contents);

    file_contents = fh.read();

    std::cout << "[" << file_contents << "]" << std::endl;
}