#include "Scanner.hpp"
#include <cctype>
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
		case '"': string(); break;
	default:
		if (std::isdigit(c))
			number();
		else if (std::isalpha(c))
			identifier();
		else
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

void Scanner::string()
{
	while (peek() != '"' && !isAtEnd())
	{
		if (peek() == '\n')
			line++;
		advance();
	}
	if (isAtEnd())
	{
		HttpParser.error(line, "Unterminated string.");
		return();
	}
	advance();
	std::string value = source.subtstring(start + 1, current - 1);
	addToken(TOKEN7, value);
}

void Scanner::number()
{
	while (std::isdigit(peek()))
		advance();

	if (peek() == '.' && std::isdigit(peekNext()))
	{
		advance();
		while (std::isdigit(peek()))
			advance();
	}
	addToken(TOKEN8, parseDouble(source.substring(start, current)));
}

void Scanner::identifier()
{
	while (std::isalnum(peek())
		advance();

	std::string text = source.substring(start, current);
	auto it = keywords.find(text);
	if (it == keywords.end())
		type = TOKEN9;
	else
		type = it->second;
	addToken(type);
}

char Scanner::peek()
{
	if (isAtEnd())
		return ('\0');
	return (source[current]);
}

char Scanner:peekNext()
{
	if (current + 1 >= source.length())
		return ('\0');
	return (source[current + 1);
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
