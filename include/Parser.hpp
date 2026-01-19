#pragma once
#include "Node.hpp"
#include "Token.hpp"
#include <memory>
#include <vector>

class Parser {
public:
  Parser(const std::vector<Token> &tokens);

  std::unique_ptr<Node> parse();

private:
  const std::vector<Token> &m_tokens;
  size_t m_pos;

  Token peek(int offset = 0) const;
  Token consume();
  bool isAtEnd() const;
  bool match(TokenType type);
  std::unique_ptr<Node> parseBlock();
  std::unique_ptr<Node> parseTable();
  std::unique_ptr<Node> parseTableRow();
  std::unique_ptr<Node> parseCodeBlock();
  std::vector<std::unique_ptr<Node>> parseInline(TokenType stopAt);
};
