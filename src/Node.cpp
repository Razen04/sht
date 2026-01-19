#include "../include/Node.hpp"
#include <iostream>
using namespace std;

void printTree(const Node &node, int depth) {
  for (int i = 0; i < depth; ++i)
    cout << "  ";

  switch (node.type) {
  case NodeType::ROOT:
    cout << "[ROOT]";
    break;
  case NodeType::PARAGRAPH:
    cout << "[P]";
    break;
  case NodeType::TABLE:
    cout << "[TABLE]";
    break;
  case NodeType::ROW:
    cout << "[ROW]";
    break;
  case NodeType::CELL:
    cout << "[CELL]";
    break;
  case NodeType::TEXT:
    cout << "'" << node.value << "'";
    break;
  case NodeType::BOLD:
    cout << "[BOLD]";
    break;
  case NodeType::ITALICS:
    cout << "[ITALIC]";
    break;
  case NodeType::IMAGE:
    cout << "[IMAGE]";
    break;
  case NodeType::INLINE_CODE:
    cout << "[INLINE_CODE]";
    break;
  case NodeType::CODE_BLOCK:
    cout << "[CODE_BLOCK]";
    break;
  default:
    cout << "[UNKNOWN]";
    break;
  }
  cout << "\n";

  for (const auto &child : node.childNodes) {
    printTree(*child, depth + 1);
  }
}
