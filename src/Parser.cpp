#include "../include/Parser.hpp"
#include "../include/Token.hpp"
#include <memory>
#include <stdexcept>
#include <vector>
using namespace std;

Parser::Parser(const vector<Token> &tokens) : m_tokens(tokens), m_pos(0) {}

Token Parser::peek(int offset) const {
  if (m_pos + offset >= m_tokens.size())
    return Token{TokenType::EOF_TOKEN, ""};
  return m_tokens[m_pos + offset];
}

// consume the character and move the pointer
Token Parser::consume() {
  if (isAtEnd())
    return Token{TokenType::EOF_TOKEN, ""};
  size_t curr = m_pos;
  m_pos++;
  return m_tokens[curr];
}

bool Parser::isAtEnd() const { return m_pos >= m_tokens.size(); }

bool Parser::match(TokenType type) {
  if (peek().type == type) {
    consume();
    return true;
  }

  return false;
}

unique_ptr<Node> Parser::parse() {
  auto root = make_unique<Node>(NodeType::ROOT);
  int safetyCtr = 0;
  while (!isAtEnd()) {
    safetyCtr++;
    if (safetyCtr > 100000) {
      throw std::runtime_error("Infinite loop in the parser.");
    }
    root->add(parseBlock());
  }

  return root;
}

unique_ptr<Node> Parser::parseBlock() {
  if (peek().type == TokenType::PIPE) {
    return parseTable();
  } else if (peek().type == TokenType::BACKTICK &&
             peek(1).type == TokenType::BACKTICK &&
             peek(2).type == TokenType::BACKTICK) {
    return parseCodeBlock();
  } else {
    auto p = make_unique<Node>(NodeType::PARAGRAPH);

    // Parse everything until we hit a NEWLINE
    auto children = parseInline(TokenType::NEWLINE);
    for (auto &child : children) {
      p->add(std::move(child));
    }

    if (peek().type == TokenType::NEWLINE) {
      consume();
    }
    return p;
  }
}

unique_ptr<Node> Parser::parseCodeBlock() {
  // consume the three initial backtick
  consume();
  consume();
  consume();

  auto codeBlock = make_unique<Node>(NodeType::CODE_BLOCK);

  // extract the language of the code
  if (peek().type == TokenType::TEXT && peek(1).type == TokenType::NEWLINE) {
    codeBlock->language += consume().value;
    // consume the next line too
    if (peek().type == TokenType::NEWLINE)
      consume();
  }

  string codeBlockValue;
  while (!isAtEnd()) {
    if (peek().type == TokenType::BACKTICK &&
        peek(1).type == TokenType::BACKTICK &&
        peek(2).type == TokenType::BACKTICK)
      break;

    if (peek().type == TokenType::NEWLINE) {
      consume();
      codeBlockValue += '\n';
    } else {
      codeBlockValue += consume().value;
    }
  }

  // consume the last three backticks
  consume();
  consume();
  consume();

  if (peek().type == TokenType::NEWLINE)
    consume();

  codeBlock->value = codeBlockValue;
  return codeBlock;
}

unique_ptr<Node> Parser::parseTable() {
  auto table = make_unique<Node>(NodeType::TABLE);

  while (peek().type == TokenType::PIPE) {
    auto children = parseTableRow();
    table->add(std::move(children));
  }

  return table;
}

unique_ptr<Node> Parser::parseTableRow() {
  auto row = make_unique<Node>(NodeType::ROW);
  consume(); // consuming the first PIPE

  while (!isAtEnd() && peek().type != TokenType::NEWLINE) {
    auto cell = make_unique<Node>(NodeType::CELL);
    auto children = parseInline(TokenType::PIPE);
    for (auto &child : children) {
      cell->add(std::move(child));
    }

    // consume the last pipe first
    consume();
    row->add(std::move(cell));
  }

  consume(); // consume the NEWLINE
  return row;
}
std::vector<std::unique_ptr<Node>> Parser::parseInline(TokenType stopAt) {
  std::vector<std::unique_ptr<Node>> nodes;

  int safety = 0;
  while (!isAtEnd()) {
    if (safety > 100000)
      throw std::runtime_error("Infinite loop in Parser inline");
    // If we are looking for "**", we must peek TWO tokens.
    if (stopAt == TokenType::DOUBLE_STAR) {
      if (peek().type == TokenType::STAR && peek(1).type == TokenType::STAR) {
        break; // Found the closing **, stop gathering children.
      }
    }
    // If we are looking for "*" or "NEWLINE", we just check the current one.
    else if (peek().type == stopAt) {
      break; // Found the closing token, stop gathering children.
    }

    // Case: Bold Start (**...)
    if (peek().type == TokenType::STAR && peek(1).type == TokenType::STAR) {
      consume(); // Eat first *
      consume(); // Eat second *

      auto boldNode = std::make_unique<Node>(NodeType::BOLD);

      // RECURSION: Call ourselves, but tell the inner parser to stop at
      // DOUBLE_STAR
      auto children = parseInline(TokenType::DOUBLE_STAR);
      for (auto &child : children)
        boldNode->add(std::move(child));
      nodes.push_back(std::move(boldNode));

      // After recursion returns, we expect to be standing on the closing tags.
      // We must consume them here.
      if (peek().type == TokenType::STAR && peek(1).type == TokenType::STAR) {
        consume();
        consume();
      }
    }

    // Case: Italic Start (*...)
    else if (peek().type == TokenType::STAR) {
      consume(); // Eat *

      auto italicNode = std::make_unique<Node>(NodeType::ITALICS);

      // RECURSION: Call ourselves, stop at single STAR
      auto children = parseInline(TokenType::STAR);
      for (auto &child : children)
        italicNode->add(std::move(child));
      nodes.push_back(std::move(italicNode));

      // Consume the closing star
      if (peek().type == TokenType::STAR) {
        consume();
      }
    }

    // image: ![]()
    else if (peek().type == TokenType::BANG &&
             peek(1).type == TokenType::OPEN_BRAC) {
      consume(); // consume the BANG
      consume(); // consume the OPEN_BRAC

      auto imageNode = make_unique<Node>(NodeType::IMAGE);

      // recursion, we stop at close_brac and for the link of image we will do
      // another recursion.
      auto children = parseInline(TokenType::CLOSE_BRAC);
      for (auto &child : children)
        imageNode->add(std::move(child));

      // consume the last close_brac
      if (peek().type == TokenType::CLOSE_BRAC)
        consume();

      // get the url of the image
      if (peek().type == TokenType::OPEN_PAREN) {
        consume();
        if (peek().type == TokenType::TEXT)
          imageNode->value = consume().value;

        if (peek().type == TokenType::CLOSE_PAREN)
          consume(); // consume the close_paren
      }

      nodes.push_back(std::move(imageNode));
    }

    // Inline Code
    else if (peek().type == TokenType::BACKTICK) {
      auto inlineCodeNode = make_unique<Node>(NodeType::INLINE_CODE);
      consume();
      string inlineCodeContent;
      while (!isAtEnd() && peek().type != TokenType::BACKTICK) {
        inlineCodeContent += consume().value;
      }

      // consume the last backtick
      consume();
      inlineCodeNode->value = inlineCodeContent;

      nodes.push_back(std::move(inlineCodeNode));
    }
    // Case: Plain Text (or unknown symbols)
    else {
      Token t = consume();
      nodes.push_back(std::make_unique<Node>(NodeType::TEXT, t.value));
    }
  }

  return nodes;
}
