#pragma once
#include "Token.hpp"
#include <string_view>

class Lexer {
public:
  Lexer(std::string_view text); // Constructor

  Token getNextToken();

private:
  std::string_view m_text;
  size_t m_pos;
  // peeking: basically seeing what are we standing on
  char peek(int offset = 0) const;

  // consume the current character and move
  char consume();

  // boolean to check are we at end
  bool isAtEnd() const;
};
