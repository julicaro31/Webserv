#include "Token.hpp"
#include <string>

std::string tokenToString(Token::TokenType tokenType) 
{
    static const std::unordered_map<Token::TokenType, std::string> tokenMap = 
	{
        {Token::SINGLE_SPACE, "SINGLE_SPACE"},
        {Token::CR, "CR"},
        {Token::LF, "LF"},
        {Token::CRLF, "CRLF"},
        {Token::IDENTIFIER, "IDENTIFIER"},
        {Token::WHITESPACE, "WHITESPACE"},
        {Token::BODY, "BODY"},
        {Token::METHOD, "METHOD"},
        {Token::URI, "URI"},
        {Token::VERSION, "VERSION"},
        {Token::HEADER, "HEADER"},
        {Token::EOFF, "EOFF"},
    };
    return tokenMap.at(tokenType);
}

std::string Token::toString(void)
{
	return (tokenToString(type) + " " + lexeme + " " + literal);
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
	out << "---------------" << std::endl;
	out << "TokenType: " << tokenToString(token.getType()) << std::endl;
	out << "Token.lexeme: " << token.getLexeme() << std::endl; 
	out	<< "Token.literal: " << token.getLiteral() << std::endl;
	out	<< "Token.line: " << token.getLine() << std::endl;
	out << "---------------" << std::endl;
	return (out);
}

Token::Token(const Token &token)
{
	this->type = token.type;
	this->lexeme = token.lexeme;
	this->literal = token.literal;
	this->line = token.line;
}

Token::Token(TokenType type, std::string lexeme, std::string literal, int line)
{
	this->type = type;
	this->lexeme = lexeme;
	this->literal = literal;
	this->line = line;
}

Token::~Token(void)
{
}

