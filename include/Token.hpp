#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <cctype>
#include <string>
#include <unordered_map>
#include <ostream>

class Token
{
public:
	enum TokenType
	{
		// single-char tokens
		SINGLE_SPACE, CR, LF,

		// one or two character tokens.
		CRLF,

		// literals.
		IDENTIFIER, WHITESPACE,
		BODY,

		// request Line
		METHOD, URI, VERSION,

		// header
		HEADER,

		// end
		END_OF_FILE,
	};
	static std::string tokenTypeToString(Token::TokenType);
	std::string toString();
	TokenType getType() const;
	std::string getLexeme() const;
	std::string getLiteral() const;
	int getLine() const;
	static std::string tokenToString(const Token& token);
	Token(TokenType, std::string, std::string, int);
	Token(const Token &);
	~Token(void);
private:
	TokenType	type;
	std::string lexeme;
	std::string	literal;
	int			line;
	static const std::unordered_map<std::string, TokenType> keywords;
};

std::ostream& operator<<(std::ostream& out, const Token& token);

#endif
