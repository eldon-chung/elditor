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
  std::vector<int> v{0, 1, 2, 3, 4};

  std::vector<int> other{10, 11, 12};
  v.erase(v.begin() + 1, v.begin() + 5);
  // v.insert(v.begin() + 6, other.begin() + 1, other.end());
  for (int x : v) {
    std::cout << x << std::endl;
  }
}