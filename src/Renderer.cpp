#include "../include/Renderer.hpp"
#include "../include/Node.hpp"
#include <string>
using namespace std;

string Renderer::escapeHtml(const string &raw) {
  int n = raw.size();

  string res;
  for (int i = 0; i < n; i++) {
    if (raw[i] == '&') {
      res += "&amp;";
    } else if (raw[i] == '<') {
      res += "&lt;";
    } else if (raw[i] == '>') {
      res += "&gt;";
    } else if (raw[i] == '"') {
      res += "&quot;";
    } else {
      res += raw[i];
    }
  }

  return res;
}

string Renderer::renderChildren(const Node *node) {
  string s;
  for (auto &child : node->childNodes) {
    s += renderNode(child.get());
  }
  return s;
}

string Renderer::renderNode(const Node *node) {
  string res;

  switch (node->type) {
  case NodeType::ROOT:
    res += "<div>" + renderChildren(node) + "</div>";
    break;
  case NodeType::PARAGRAPH:
    if (node->childNodes.empty() && node->value.empty()) {
      break;
    }
    res += "<p>" + renderChildren(node) + "</p>";
    break;
  case NodeType::CODE_BLOCK:
    res += "<pre>";
    res += "<code class=\"language-" + node->language + "\">";
    res += escapeHtml(node->value);
    res += "</code>";
    res += "</pre>";
    break;
  case NodeType::BOLD:
    res += "<b>" + renderChildren(node) + "</b>";
    break;
  case NodeType::ITALICS:
    res += "<i>" + renderChildren(node) + "</i>";
    break;
  case NodeType::TEXT:
    res += escapeHtml(node->value);
    break;
  case NodeType::IMAGE:
    res += "<img class=\"image\""
           " src=\"" +
           escapeHtml(node->value) +
           "\""
           " alt=\"" +
           renderChildren(node) +
           "\""
           "/><br><em>" +
           renderChildren(node) + "</em>";
    break;
  case NodeType::INLINE_CODE:
    res += "<span class=\"inline-code\">" + escapeHtml(node->value) + "</span>";
    break;
  case NodeType::TABLE:
    res += "<table><tbody>" + renderChildren(node) + "</tbody></table>";
    break;
  case NodeType::ROW:
    res += "<tr>" + renderChildren(node) + "</tr>";
    break;
  case NodeType::CELL:
    res += "<td>" + renderChildren(node) + "</td>";
    break;
  case NodeType::NEWLINE:
    res += "<br>";
    break;
  default:
    res += "";
    break;
  }

  return res;
}

string Renderer::render(const Node &root) { return renderNode(&root); }
