#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <cctype>
#include <string>
#include <unordered_map>

class Token
{
public:
	enum TokenType
	{
		// single-char tokens
		DOT, SLASH, QUESTION_MARK, EQUAL,
		SINGLE_SPACE, COLON, STAR, CR, LF,

		// One or two character tokens.
		DOUBLE_SLASH, CRLF,

		// Literals.
		IDENTIFIER, STRING, NUMBER, METHOD,
		REQUEST_TARGET, HTTP_VERSION, WHITESPACE,

		// Keywords.
		WHERE,
		EOF,
	};
private:
	TokenType	type;
	std::string lexeme;
	std::string	literal;
	int			line;
	static const std::unordered_map<std::string, TokenType> keywords;
public:
	std::string toString();
	Token(TokenType, std::string, std::string, int);
	Token(const Token &);
};

#endif
