#pragma once
#include "Node.hpp"
#include <string>
class Renderer {
public:
  std::string render(const Node &root);

private:
  std::string renderNode(const Node *node);
  std::string renderChildren(const Node *node);

  std::string escapeHtml(const std::string_view &raw);
};
