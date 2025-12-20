#include "../include/Lexer.hpp"
#include <iostream>
using namespace std;

int main() {
  string src = "**Hello** *world*!";
  Lexer lx(src);

  // Get all the characters printed
	Token token = lx.getNextToken();
  while (token.type != TokenType::EOF_TOKEN) {
    cout << token.value << '\n';
		token = lx.getNextToken();
  }
  return 0;
}
