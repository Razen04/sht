#pragma once
#include <string_view>

enum class TokenType {
  MATH_INLINE,
  MATH_BLOCK,
  STAR,
  DOUBLE_STAR,
  TEXT,
  PIPE,
  DASH,
  BANG,
  OPEN_BRAC,
  CLOSE_BRAC,
  OPEN_PAREN,
  CLOSE_PAREN,
  BACKTICK,
  NEWLINE,
  EOF_TOKEN
};

struct Token {
  TokenType type;
  std::string_view value;
};
