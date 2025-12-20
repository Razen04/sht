#include "../include/Lexer.hpp"
#include <string_view>
using namespace std;

Lexer::Lexer(string_view text) {
  m_text = text;
  m_pos = 0;
}

char Lexer::peek(int offset) const {
  if (m_pos + offset >= m_text.size())
    return '\0';
  return m_text[m_pos + offset];
}

// consume the character and move the pointer
char Lexer::consume() {
  if (isAtEnd())
    return '\0';
  int curr = m_pos;
  m_pos++;
  return m_text[curr];
}

bool Lexer::isAtEnd() const { return m_pos >= m_text.size(); }

Token Lexer::getNextToken() {
  if (isAtEnd()) {
    return Token{TokenType::EOF_TOKEN, ""};
  }
	char start = m_pos;
	consume();
  return Token{TokenType::DOUBLE_STAR, m_text.substr(start, 1)};
}
