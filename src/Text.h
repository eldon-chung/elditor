#pragma once

#include <string>
#include <string_view>
#include <vector>

struct Text {
  // not sure if i should turn this into string_view
  std::string m_text;
  std::vector<size_t> m_line_lengths;

  Text(std::string &&text, std::vector<size_t> &&m_line_lengths)
      : m_text(std::move(text)), m_line_lengths(std::move(m_line_lengths)) {
  }

  // remove the copy constructor and assignment operator
  // we want this to be a move only thing
  Text(Text const &) = delete;
  Text &operator=(Text const &) = delete;
  ~Text() {
  }

  size_t line_length_at(size_t line_idx) const {
    return m_line_lengths[line_idx];
  }

  std::string_view get_line_at(size_t line_idx) const {
    // this solution is quite slow right now
    size_t left_idx = 0;
    for (size_t idx = 0; idx < line_idx; idx++) {
      left_idx += m_line_lengths.at(idx) + 1;
    }
    size_t right_idx = left_idx + m_line_lengths.at(line_idx);

    return std::string_view{m_text.data() + left_idx,
                            m_text.data() + right_idx};
  }

  size_t num_lines() const {
    return m_line_lengths.size();
  }
};