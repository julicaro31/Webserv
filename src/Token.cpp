#include "Token.hpp"
#include <string>

std::string Token::toString(void)
{
	return (lexeme + " " + literal);
}

Token::TokenType Token::getType() const
{
	return (type);
}

std::string Token::getLexeme() const
{
	return (lexeme);
}

std::string Token::getLiteral() const
{
	return (literal);
}

int Token::getLine() const
{
	return (line);
}

std::ostream& operator<<(std::ostream& out, const Token& token)
{
	out << "TokenType: " << token.getType() << " | " << "Token.lexeme: " << token.getLexeme() << " | " << "Token.literal: " << token.getLiteral() << " | " << "Token.line: " << token.getLine() << std::endl;
	return (out);
}

Token::Token(TokenType type, std::string lexeme, std::string literal, int line)
{
	this->type = type;
	this->lexeme = lexeme;
	this->literal = literal;
	this->line = line;
}

Token::Token(const Token &token)
{
	this->type = token.type;
	this->lexeme = token.lexeme;
	this->literal = token.literal;
	this->line = token.line;
}
