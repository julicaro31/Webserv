#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <cctype>
#include <string>
#include "TokenType.hpp"

class Token
{
private:
	TokenType	type;
	std::string lexeme;
	std::string	literal;
	int			line;
public:
	std::string toString();
	Token(TokenType, std::string, std::string, int);
	Token(const Token &);
};

#endif
