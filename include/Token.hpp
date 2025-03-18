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
		SINGLE_SPACE,

		// one or two character tokens.
		CRLF,

		// literals.
		IDENTIFIER, STRING, NUMBER, 
		WHITESPACE,

		// request Line
		METHOD, URI, VERSION,

		// response Line
		STATUS_CODE, REASON_PHRASE,

		// header
		HEADER_NAME, HEADER_VALUE,

		// keywords
		GET, POST, DELETE, PUT, HEAD, OPTIONS, CONNECT, TRACE, PATCH,

		// end
		EOFF,
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
