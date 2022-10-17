#pragma once

#include <cassert>
#include <ncurses.h>
#include <string>
#include <string_view>
#include <vector>

#include "Colours.h"

struct TextTag {
    // exclusive ranges
    size_t m_start_pos;
    size_t m_end_pos;

    COLOUR m_colour;
    ATTRIBUTE m_attribute;

    TextTag(size_t start_pos, size_t end_pos, COLOUR colour = COLOUR::NORMAL,
            ATTRIBUTE attribute = ATTRIBUTE::NORMAL)
        : m_start_pos(start_pos), m_end_pos(end_pos), m_colour(colour), m_attribute(attribute) {
    }

    size_t length() const {
        assert(m_end_pos >= m_start_pos);
        return m_end_pos - m_start_pos;
    }
};

struct Text {
    std::string m_text;

    std::vector<size_t> m_line_lengths;
    std::vector<size_t> m_line_pos;

    Text(std::string &&text, std::vector<size_t> &&line_lengths, std::vector<size_t> line_pos)
        : m_text(std::move(text)), m_line_lengths(std::move(line_lengths)), m_line_pos(line_pos) {
    }

    // remove the copy constructor and assignment operator
    // we want this to be a move only thing
    Text(Text const &) = delete;
    Text &operator=(Text const &) = delete;
    ~Text() {
    }

    size_t line_length_at(size_t line_idx) const {
        return m_line_lengths.at(line_idx);
    }

    std::string_view get_line_at(size_t line_idx) const {
        size_t starting_idx = m_line_pos.at(line_idx);
        return std::string_view{m_text.data() + starting_idx, m_line_lengths.at(line_idx)};
    }

    size_t num_lines() const {
        return m_line_lengths.size();
    }

    size_t size() const {
        return m_text.size();
    }
};

class TaggedText {
    std::string m_text;
    std::vector<TextTag> m_tags;

  public:
    TaggedText(std::string text = "") : m_text(text) {
    }

    TaggedText(std::string text, std::vector<TextTag> tags)
        : m_text(std::move(text)), m_tags(std::move(tags)) {
    }

    // you should be using this to get the resulting text
    std::string_view get_text() {
        return std::string_view{m_text};
    }

    void resize(size_t size) {
        m_text.resize(size);
    }

    void remove_prefix(size_t size) {
        if (size >= m_text.size()) {
            m_text.clear();
        } else {
            std::string_view temp_view{m_text};
            temp_view.remove_prefix(size);
            m_text = std::string{temp_view};
        }
    }

    TextTag &get_tag(size_t index) {
        return m_tags.at(index);
    }

    TextTag get_const_tag(size_t index) const {
        return m_tags.at(index);
    }

    void add_tag(TextTag text_tag) {
        m_tags.push_back(std::move(text_tag));
    }

    std::vector<TextTag> &get_tags() {
        return m_tags;
    }

    std::vector<TextTag> const &get_tags() const {
        return m_tags;
    }

    size_t size() const {
        return m_text.size();
    }

    friend std::ostream &operator<<(std::ostream &os, TaggedText const &text) {
        os << text.m_text;
        return os;
    }
};
