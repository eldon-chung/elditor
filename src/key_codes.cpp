#include "key_codes.h"
// Takes in a keycode from ncurses and reinterprets this as a key struct
//
std::optional<Key> keycode_to_key(int keycode) {

  // Check first to see if the keycode has been reserved
  if (reserved_keycode_to_key.contains(keycode)) {
    return reserved_keycode_to_key.at(keycode);
  }

  if (std::isalpha(keycode)) {
    return Key(keycode, KeyType::ALPHA, KeyModifier::NONE);
  } else if (std::isdigit(keycode)) {
    return Key(keycode, KeyType::DIGIT, KeyModifier::NONE);
  } else if (std::ispunct(keycode)) {
    return Key(keycode, KeyType::PUNCTUATION, KeyModifier::NONE);
  }

  // const char *keyname_str = keyname(keycode);
  // if (keyname_str[0] == '^' && std::isalpha(keyname_str[1]) &&
  //     keyname_str[2] == '\0') {
  //   return Key(keycode, KeyType::ALPHA, KeyModifier::CTRL);
  // }

  // Otherwise we ignore the keycode
  return std::nullopt;
}