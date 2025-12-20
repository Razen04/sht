#pragma once
#include <string_view>
using namespace std;

enum class TokenType {
	STAR,
	DOUBLE_STAR,
	EOF_TOKEN
};

struct Token {
	TokenType type;
	string_view value;
};
