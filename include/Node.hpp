#pragma once
#include "string_view"
#include <memory>
#include <vector>

enum class NodeType {
  ROOT,
  PARAGRAPH,
  TABLE,
  ROW,
  CELL,
  BOLD,
  ITALICS,
  TEXT,
  IMAGE,
  INLINE_CODE,
  CODE_BLOCK,
  NEWLINE
};

struct Node {
  NodeType type;
  std::string_view value; // small optimisation
  std::string language;   // optional, only for CODE_BLOCK
  std::vector<std::unique_ptr<Node>> childNodes;

  Node(NodeType t, std::string_view v = "") : type(t), value(v) {};

  // add nodes helper
  void add(std::unique_ptr<Node> child) {
    childNodes.push_back(std::move(child));
  }
};

void printTree(const Node &node, int depth = 0);
