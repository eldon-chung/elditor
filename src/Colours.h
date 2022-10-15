#pragma once

#include <ncurses.h>

enum class COLOUR {
  NORMAL,
  CURSOR,
};

enum class ATTRIBUTE {
  NORMAL = A_NORMAL,
  HIGHLIGHT = A_STANDOUT,
  UNDERLINE = A_UNDERLINE,
  BOLD = A_BOLD,
  DIM = A_DIM,
};