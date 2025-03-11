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
		case '!': addToken(match('=') ? TOKEN4 : TOKEN5);
		case '/': 
			if (match('/'))
				while (peek() != '\n' && !isAtEnd()) advance();
			else 
				addToken(TOKEN6)
			break;
		case ' ': break;
		case '\r': break;
		case '\t': break;
		case '\n': line++; break;
	default:
		HttpParser.error(line, "Unexpected character."); break;
	}
}

char Scanner::advance()
{
	return (source[current++]);
}

bool Scanner::match(char expected)
{
	if (isAtEnd())
		return (false);
	else if (source[current] != expected)
		return (false);
	
	current++;
	return (true);
}

char Scanner::peek()
{
	if (isAtEnd())
		return ('\0');
	return (source[current]);
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
