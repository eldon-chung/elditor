#include <cctype>
#include <iostream>
#include <ncurses.h>
#include <optional>
#include <unordered_map>
#pragma once
/* Basic control codes. */
#define ESC_CODE 27
#define DELETE_CODE 330
#define BACKSPACE_CODE 263
#define SPACE_CODE 32
#define TAB_CODE 9
#define ENTER_CODE 10
#define UP 259
#define DOWN 258
#define LEFT 260
#define RIGHT 261

/* subset of special xterm key_codes */
// ARROW KEYS
// CTRL
#define CONTROL_LEFT 546
#define CONTROL_RIGHT 561
#define CONTROL_UP 567
#define CONTROL_DOWN 526

// SHIFT CTRL
#define SHIFT_CONTROL_LEFT 547
#define SHIFT_CONTROL_RIGHT 562
#define SHIFT_CONTROL_UP 568
#define SHIFT_CONTROL_DOWN 527

// ALT
#define ALT_UP 565
#define ALT_DOWN 524

// SHIFT
#define SHIFT_LEFT 393
#define SHIFT_RIGHT 402
#define SHIFT_UP 337
#define SHIFT_DOWN 336

// TAB
#define SHIFT_TAB 353

// DELETE
#define CONTROL_DELETE 520

// BACKSPACE
#define CONTROL_BACKSPACE 8

// Special key combinations; only have to list the non alphabetical ones
#define CONTROL_SLASH 31
#define CONTROL_Q 17

enum KeyType {
    ALPHA,
    DIGIT,
    PUNCTUATION,
    ARROW,
    DELETE,
    BACKSPACE,
    TAB,
    SPACE,
    ENTER,
    ESCAPE,
};

// Not sure if i should just enum all combinations I want to handle
// which arent that many
enum KeyModifier {
    NONE,
    CTRL,
    ALT,
    SHIFT,
    CTRL_SHIFT
};

// digits, alphas, and punctuations are writable
// everything else should be intercepted
// what keyname do we give?
struct Key {
    int m_keycode;
    KeyType m_keytype;
    KeyModifier m_modifier;

    Key(int keycode, KeyType keytype, KeyModifier modifier)
        : m_keycode(keycode), m_keytype(keytype), m_modifier(modifier) {
    }

    bool is_modified() const {
        return !is_modified_by(KeyModifier::NONE);
    }

    bool is_modified_by(const KeyModifier modifier) const {
        return m_modifier == modifier;
    }

    bool is_insertable() const {
        return !is_modified() &&
               (std::isalpha(m_keycode) || std::isdigit(m_keycode) || std::ispunct(m_keycode) ||
                m_keytype == KeyType::SPACE || m_keytype == KeyType::TAB);
    }

    std::string to_string() const {
        std::string output_string{""};
        output_string.append("keycode: ");
        output_string.append(std::to_string(m_keycode));
        output_string.append(" keytype: ");
        output_string.append(std::to_string(m_keytype));
        output_string.append(" modifier: ");
        output_string.append(std::to_string(m_modifier));
        return output_string;
    }

    bool has_keycode(int keycode) const {
        return m_keycode == keycode;
    }

    bool is_type(KeyType key_type) const {
        return m_keytype == key_type;
    }

    KeyType type() const {
        return m_keytype;
    }

    KeyModifier modifier() const {
        return m_modifier;
    }

    // should only call this when it is an insertable type
    char get_char() const {
        return (char)m_keycode;
    }

    // friend std::ostream &operator<<(std::ostream &os, const Key &key);
};

// std::ostream &operator<<(std::ostream &os, const Key &key) {
//   os << "keycode: " << key.m_keycode << " keytype: " << key.m_keytype
//      << " modifier: " << key.m_modifier;
//   return os;
// }

/***
 * The mapping rules are as follows:
 * There is a base set of allowed keys. Which is exactly alphabets,
 * digits, and punctuation.
 * There is a special set of allowed keys. Which is exactly {backspace, space,
 * tab, escape, arrow keys, enter, delete}.
 *
 * If it is unmodified from the base set. Construct it directly.
 *
 * If it is Ctrl + alphabet = store the received keycode as is, with
 * KeyType::ALPHA_UPPER, Modifier::CTRL.
 *
 * Otherwise, reserved_keycode_to_key retains all the remaining allowed
 * mappings.
 * If it does not exist here, then the keycode should be ignored (no key struct
 * should be made for it)
 */
const std::unordered_map<int, const Key> reserved_keycode_to_key{
    // arrow keys and their modifiers
    {UP, {UP, KeyType::ARROW, KeyModifier::NONE}},
    {DOWN, {DOWN, KeyType::ARROW, KeyModifier::NONE}},
    {LEFT, {LEFT, KeyType::ARROW, KeyModifier::NONE}},
    {RIGHT, {RIGHT, KeyType::ARROW, KeyModifier::NONE}},
    {CONTROL_UP, {CONTROL_UP, KeyType::ARROW, KeyModifier::CTRL}},
    {CONTROL_DOWN, {CONTROL_DOWN, KeyType::ARROW, KeyModifier::CTRL}},
    {CONTROL_LEFT, {CONTROL_LEFT, KeyType::ARROW, KeyModifier::CTRL}},
    {CONTROL_RIGHT, {CONTROL_RIGHT, KeyType::ARROW, KeyModifier::CTRL}},
    {SHIFT_UP, {SHIFT_UP, KeyType::ARROW, KeyModifier::SHIFT}},
    {SHIFT_DOWN, {SHIFT_DOWN, KeyType::ARROW, KeyModifier::SHIFT}},
    {SHIFT_LEFT, {SHIFT_LEFT, KeyType::ARROW, KeyModifier::SHIFT}},
    {SHIFT_RIGHT, {SHIFT_RIGHT, KeyType::ARROW, KeyModifier::SHIFT}},
    {SHIFT_CONTROL_UP, {SHIFT_CONTROL_UP, KeyType::ARROW, KeyModifier::CTRL_SHIFT}},
    {SHIFT_CONTROL_DOWN, {SHIFT_CONTROL_DOWN, KeyType::ARROW, KeyModifier::CTRL_SHIFT}},
    {SHIFT_CONTROL_LEFT, {SHIFT_CONTROL_LEFT, KeyType::ARROW, KeyModifier::CTRL_SHIFT}},
    {SHIFT_CONTROL_RIGHT, {SHIFT_CONTROL_RIGHT, KeyType::ARROW, KeyModifier::CTRL_SHIFT}},
    {ALT_UP, {ALT_UP, KeyType::ARROW, KeyModifier::ALT}},
    {ALT_DOWN, {ALT_DOWN, KeyType::ARROW, KeyModifier::ALT}},
    // backspace, and their modifiers
    {BACKSPACE_CODE, {BACKSPACE_CODE, KeyType::BACKSPACE, KeyModifier::NONE}},
    {CONTROL_BACKSPACE, {CONTROL_BACKSPACE, KeyType::BACKSPACE, KeyModifier::CTRL}},
    // tab, and their modifiers
    {TAB_CODE, {TAB_CODE, KeyType::TAB, KeyModifier::NONE}},
    {SHIFT_TAB, {SHIFT_TAB, KeyType::TAB, KeyModifier::SHIFT}},
    // delete, and their modifiers
    {DELETE_CODE, {DELETE_CODE, KeyType::DELETE, KeyModifier::NONE}},
    {CONTROL_DELETE, {CONTROL_DELETE, KeyType::DELETE, KeyModifier::CTRL}},
    // space, and their modifiers
    {SPACE_CODE, {SPACE_CODE, KeyType::SPACE, KeyModifier::NONE}},
    // esc, and their modifiers
    {ESC_CODE, {ESC_CODE, KeyType::ESCAPE, KeyModifier::NONE}},
    // enter, and their modifiers
    {ENTER_CODE, {ENTER_CODE, KeyType::ENTER, KeyModifier::NONE}},
    // MISC Key combinations
    {CONTROL_SLASH, {'/', KeyType::PUNCTUATION, KeyModifier::CTRL}},
    {CONTROL_Q, {'Q', KeyType::ALPHA, KeyModifier::CTRL}},
};

std::optional<Key> keycode_to_key(int keycode);