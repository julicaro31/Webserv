#include "Token.hpp"
#include <string>

std::string Token::tokenTypeToString(Token::TokenType tokenType) 
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
        {Token::END_OF_FILE, "EOF"},
    };
    return tokenMap.at(tokenType);
}

std::string Token::toString(void)
{
	return (tokenTypeToString(type) + " " + lexeme + " " + literal);
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
	out << "TokenType: " << token.tokenTypeToString(token.getType()) << std::endl;
	out << "Token.lexeme: " << token.getLexeme() << std::endl; 
	out	<< "Token.literal: " << token.getLiteral() << std::endl;
	out	<< "Token.line: " << token.getLine() << std::endl;
	out << "---------------" << std::endl;
	return (out);
}

std::string Token::tokenToString(const Token& token)
{
	std::string ret("\n***************\n");
	ret.append("TokenType " + token.tokenTypeToString(token.getType()) + "\n" );
	ret.append("Token.lexeme: " + token.getLexeme() + "\n" );
	ret.append("Token.literal: " + token.getLiteral() + "\n" );
	ret.append("Token.line" + std::to_string(token.getLine()) + "\n" );
	ret.append("---------------\n" );
	return (ret);
}

Token::Token(const Token &token)
	: type {token.type}
	, lexeme {token.lexeme}
	, literal {token.literal}
	, line {token.line}
{
}

Token::Token(TokenType type, std::string lexeme, std::string literal, int line)
	: type {type}
	, lexeme {lexeme}
	, literal {literal}
	, line {line}
{
}

Token::~Token(void)
{
}

