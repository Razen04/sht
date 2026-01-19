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
  int safetyCtr = 0;
  if (isAtEnd()) {
    return Token{TokenType::EOF_TOKEN, ""};
  }
  size_t start = m_pos;
  char current = peek();

  if (current == '$' && peek(1) == '$') {
    consume();
    consume();

    int safetyCtr = 0;
    while (!isAtEnd()) {
      safetyCtr++;
      if (safetyCtr > 100000) {
        throw std::runtime_error("Infinite loop in Lexer");
      }

      if (peek() == '$' && peek(1) == '$') {
        break;
      }

      consume();
    }

    consume();
    consume();

    return Token{TokenType::TEXT, m_text.substr(start, (m_pos - start))};
  }

  if (current == '$') {
    consume();

    int safetyCtr = 0;
    while (!isAtEnd()) {
      safetyCtr++;
      if (safetyCtr > 100000) {
        throw std::runtime_error("Infinite loop in Lexer");
      }

      if (peek() == '$')
        break;
      consume();
    }

    consume();
    return Token{TokenType::TEXT, m_text.substr(start, (m_pos - start))};
  }

  // i will just return star here, the parser will think what to do with the
  // stars and the lexer will be dumb(like me)
  if (current == '*') {
    consume();
    return Token{TokenType::STAR, "*"};
  }

  // next we will take care of other markdown things like tables, image, code
  // first tables: a markdown has a table structure like |A|B|--|--|1|2|, so
  // first PIPE
  if (current == '|') {
    consume();
    return Token{TokenType::PIPE, "|"};
  }
  // next DASH for header seperation in tables
  if (current == '-') {
    consume();
    return Token{TokenType::DASH, "-"};
  }

  // next is image: in markdown all the images are in format: ![](), so a BANG,
  // OPEN_BRAC, CLOSE_BRAC, OPEN_PAREN, CLOSE_PAREN
  if (current == '!') {
    consume();
    return Token{TokenType::BANG, "!"};
  }

  if (current == '[') {
    consume();
    return Token{TokenType::OPEN_BRAC, "["};
  }

  if (current == ']') {
    consume();
    return Token{TokenType::CLOSE_BRAC, "]"};
  }

  if (current == '(') {
    consume();
    return Token{TokenType::OPEN_PAREN, "("};
  }

  if (current == ')') {
    consume();
    return Token{TokenType::CLOSE_PAREN, ")"};
  }

  // next is code, for code I will be looking at the ```: the three backticks
  // for full code and `: single backtick for inline code, but this will be done
  // by parser, the lexer will just emit BACK_TICK
  if (current == '`') {
    consume();
    return Token{TokenType::BACKTICK, "`"};
  }

  // newline
  if (current == '\n') {
    consume();
    return Token{TokenType::NEWLINE, "\n"};
  }

  while (!isAtEnd()) {
    safetyCtr++;
    if (safetyCtr > 100000) {
      throw std::runtime_error("Infinite loop in Lexer");
    }
    char c = peek();

    if (c == '*' || c == '|' || c == '-' || c == '!' || c == '[' || c == ']' ||
        c == '(' || c == ')' || c == '`' || c == '\n' || c == '$')
      break;

    consume();
  }

  return Token{TokenType::TEXT, m_text.substr(start, (m_pos - start))};
}
