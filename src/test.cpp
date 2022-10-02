#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

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
  std::string str{"abcdef"};

  std::vector<std::string_view> line_list = break_lines(str);
  std::cout << "printing lines" << std::endl;
  for (std::string_view line : line_list) {
    std::cout << "[" << line << "]" << std::endl;
  }

  std::cout << (str == line_list.at(0)) << std::endl;

  // std::string halved{str, 4};
  // std::cout << "[" << halved << "]" << std::endl;
  // str.resize(4);
  // std::cout << "[" << str << "]" << std::endl;
  // std::vector<int> original{0, 1, 2, 3, 4, 5};

  // std::vector<int> middle{50, 51, 52};
  // original.insert(original.begin() + 6, middle.begin() + 1, middle.end());

  // std::cout << "[";
  // for (int x : original) {
  //   std::cout << x << ",";
  // }
  // std::cout << "]";

  // std::string original{"0123456"};
  // std::string mid{"<middle part>"};

  // std::string_view view{original.begin() + 2, original.end()};
  // std::cout << view << std::endl;
  // original.insert(2, mid);
  // std::cout << original << std::endl;
}