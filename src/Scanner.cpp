#include "Scanner.hpp"
#include <vector>
#include "Token.hpp"

std::vector<Token> Scanner::scanTokens(void)
{
	while (!isAtEnd())
	{
		start = current;
		scanToken();
	}
	tokens.push_back(Token(EOF, "", nullptr, line));
	return (tokens);
}

bool Scanner::isAtEnd(void)
{
	return (current >= source.length());
}

void Scanner::scanToken()
{
	char c = advance();
	switch (c){
		case 'a': addToken(TOKEN1); break;
		case 'b': addToken(TOKEN2); break;
		case 'c': addToken(TOKEN3); break;
	default:
		HttpParser.error(line, "Unexpected character."); break;
	}
}

char Scanner::advance ()
{
	return (source[current++]);
}

void Scanner::addToken(TokenType type)
{
	addToken(type, nullptr);
}

void Scanner::addToken(TokenType type, std::string literal)
{
	std::string text = source.substring(start, current);
	tokens.push_back(Token(type, text, literal, line));

Scanner::Scanner(std::string source)
{
	this->source = source;
}

Scanner::Scanner(const Scanner &scanner)
{
}
