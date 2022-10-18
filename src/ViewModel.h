#pragma once

#include "Model.h"

class ViewModel {
    Model *const m_model;
    // keep track of relevant data for the view class
    std::vector<TaggedText> m_tagged_text;

  public:
    ViewModel(Model *const model) : m_model(model) {
    }
    ~ViewModel(){};
    ViewModel(ViewModel const &) = delete;
    ViewModel(ViewModel &&) = delete;

    ViewModel &operator=(ViewModel const &) = delete;
    ViewModel &operator=(ViewModel &&) = delete;

    void prepare_view_data() {
        update_tagged_text();
    }

    // Getters for the view
    Cursor get_cursor() const {
        return m_model->get_cursor();
    }

    std::vector<TaggedText> const &get_const_tagged_text() const {
        return m_tagged_text;
    }

    std::vector<TaggedText> get_tagged_text() const {
        return m_tagged_text;
    }

    TaggedText const &get_const_tagged_line_at(size_t index) const {
        return m_tagged_text.at(index);
    }

    TaggedText get_tagged_line_at(size_t index) const {
        return m_tagged_text.at(index);
    }

    size_t num_lines() const {
        return m_tagged_text.size();
    }

  private:
    // Gets text from the model and prepares it with tags etc
    void update_tagged_text() {
        // get the text from the model
        Text text = m_model->get_text();

        // prepare the tagged_text
        m_tagged_text.clear();
        m_tagged_text.reserve(text.num_lines());
        for (size_t line_idx = 0; line_idx < text.num_lines(); ++line_idx) {
            m_tagged_text.push_back(std::string{text.get_line_at(line_idx)});
        }

        // tag the respective lines with the cursor tags
        add_cursor_tag();
    }

    void add_cursor_tag() {
        // assert that by the time this is called, tagged_text should at least not be empty
        assert(!m_tagged_text.empty());

        // create the cursor_tags
        Cursor cursor = m_model->get_cursor();
        if (cursor.in_selection_mode()) {
            // if the cursor is in selection mode we have to tag multiple lines
            std::pair<CursorPoint, CursorPoint> point_pair = cursor.get_const_points_in_order();
            CursorPoint left_point = point_pair.first;
            CursorPoint right_point = point_pair.second;
            if (left_point.row() == right_point.row()) {
                // single row logic
                assert(left_point.col() < right_point.col());
                TextTag text_tag{left_point.col(), right_point.col(), COLOUR::NORMAL, ATTRIBUTE::UNDERLINE};
                m_tagged_text.at(left_point.row()).add_tag(std::move(text_tag));
            } else {
                // multi row logic
                assert(left_point.row() < right_point.row());
                size_t end_pos = m_tagged_text.at(left_point.row()).size();
                // tag for the first row
                m_tagged_text.at(left_point.row())
                    .add_tag({left_point.col(), end_pos, COLOUR::NORMAL, ATTRIBUTE::UNDERLINE});
                // tag for the middle rows
                for (size_t idx = left_point.row() + 1; idx < right_point.row(); ++idx) {
                    end_pos = m_tagged_text.at(idx).size();
                    m_tagged_text.at(idx).add_tag({0, end_pos, COLOUR::NORMAL, ATTRIBUTE::UNDERLINE});
                }
                // tag for the final row
                m_tagged_text.at(right_point.row())
                    .add_tag({0, right_point.col(), COLOUR::NORMAL, ATTRIBUTE::UNDERLINE});
            }
        } else {
            assert(!cursor.in_selection_mode());
            TextTag text_tag(cursor.col(), cursor.col() + 1, COLOUR::NORMAL, ATTRIBUTE::HIGHLIGHT);
            m_tagged_text.at(cursor.row()).add_tag(std::move(text_tag));
        }
    }
};