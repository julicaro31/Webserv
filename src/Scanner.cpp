#include "Scanner.hpp"
#include <cctype>
#include <vector>
#include "HttpParser.hpp"
#include <iostream>

std::vector<Token> Scanner::scanTokens(void)
{
	while (!isAtEnd())
	{
		start = current;
		scanToken();
	}
	tokens.push_back(Token(Token::EOFF, "", "", line));
	return (tokens);
}

bool Scanner::isAtEnd(void)
{
	return (current >= static_cast<int>(source.length()));
}

bool Scanner::isAtEnd(int index)
{
	return (current + index >= static_cast<int>(source.length()));
}

void Scanner::scanToken()
{
	char c = advance();
	switch (c){
		case '*': if (peek() == ' ') addToken(Token::URI); break;
		case '/': uri(); break;
		case ' ': 
			if (match(' '))
			{
				addToken(Token::WHITESPACE);
				while (peek() == ' ') advance();
			}
			else
				addToken(Token::SINGLE_SPACE);
			break;
		case '\r': 
			if (match('\n'))
			{
				addToken(Token::CRLF);
				line++;
			}
			else
				addToken(Token::WHITESPACE);
			break;
		case '\t': addToken(Token::WHITESPACE); break;
		case '\v': addToken(Token::WHITESPACE); break;
		case '\f': addToken(Token::WHITESPACE); break;
		case '\n': line++; break;
		case '"': string(); break;
		case 'H': version(); break;
		case 'h': uri(); break;
	default:
		if (std::isdigit(c))
			number();
		else if (std::isalpha(c))
			identifier();
		else if (header());
		else
			HttpParser::error(line, "Unexpected character.");
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

void Scanner::version()
{
	int start = current;

	if (peek() != 'H' || peek(1) != 'T' || peek(2) != 'T' || peek(3) != 'P')
		return ;
	if (peek(4) != '/') return ;
	if (!std::isdigit(peek(5))) return ;
	if (peek(6) != '.') return ;
	if (!std::isdigit(peek(7))) return ;
	if (peek(8) != ' ') return ;
	current += 9;
	std::string value = source.substr(start + 1, (current - 1) - start);
	addToken(Token::VERSION, value);
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
		HttpParser::error(line, "Unterminated string.");
		return;
	}
	advance();
	std::string value = source.substr(start + 1, (current - 1) - start);
	addToken(Token::STRING, value);
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
	addToken(Token::NUMBER, source.substr(start, current - start));
}

void Scanner::uri()
{
	int i = 0;
	if (peek() == '/')
	{
		while (peek(i) != ' ' && !isAtEnd(i))
		{
			if (peek(i) == '\n')
			{
				HttpParser::error(line, "Wrong URI origin-form format");
				return;
			}
			++i;
		}
		if (isAtEnd(i))
		{
			HttpParser::error(line, "Wrong URI origin-form format");
			return;
		}
		std::string value = source.substr(start, (current + i) - start);
		addToken(Token::URI, value);
	}
	if (peek() == 'h' || peek(1) == 't' || peek(2) == 't' || peek(3) == 'p')
	{
		if (peek(4) != '/') return;
		if (peek(5) != '/') return;
		i = 5;
		while (peek(i) != ' ' && !isAtEnd(i))
		{
			if (peek(i) == '\n')
			{
				HttpParser::error(line, "Wrong URI absolute-form format");
				return;
			}
			++i;
		}
		if (isAtEnd(i))
		{
			HttpParser::error(line, "Wrong URI absolute-form format");
			return;
		}
		std::string value = source.substr(start, (current + i) - start);
		addToken(Token::URI, value);
	}
}

bool Scanner::header()
{
	int i = 0;
	int header_index = 0;
	if (!std::isalpha(peek()))
		return (false);
	while (peek(i) != ':' && !isAtEnd(i))
	{
		if (peek(i) == '\n')
		{
			HttpParser::error(line, "Wrong header_name format");
			return (false);
		}
		++i;
	}
	if (isAtEnd(i))
	{
		HttpParser::error(line, "Wrong header_name format");
		return (false);
	}
	if (peek(i) == ':' && peek(i + 1) == ' ')
		header_index = i;
	else
		return (false);
	while (peek(i) != '\n' && !isAtEnd(i))
		++i;
	if (isAtEnd(i))
	{
		HttpParser::error(line, "Wrong header_value format");
		return (false);
	}
	std::string header_name = source.substr(start, (current + header_index) - start);
	std::string header_value = source.substr(current + header_index, (current + i) - (current + header_index));
	addToken(Token::HEADER_NAME, header_name);
	addToken(Token::HEADER_VALUE, header_value);
	return (false);
}

void Scanner::identifier()
{
	while (std::isalnum(peek()))
		advance();
	std::string text = source.substr(start, current - start);
	auto it = keywords.find(text);
	Token::TokenType type;
	if (it == keywords.end())
		type = Token::IDENTIFIER;
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

char Scanner::peek(int index)
{
	if (isAtEnd())
		return ('\0');
	return (source[current + index]);
}

char Scanner::peekNext()
{
	if (current + 1 >= static_cast<int>(source.length()))
		return ('\0');
	return (source[current + 1]);
}

void Scanner::addToken(Token::TokenType type)
{
	addToken(type, "");
}

void Scanner::addToken(Token::TokenType type, std::string literal)
{
	std::cout << "-----------------------------" << std::endl;
	std::cout << "start: " << start << std::endl;
	std::cout << "current: " << current << std::endl;
	std::string text = source.substr(start, current - start);
	std::cout << "text: " << text << std::endl;
	tokens.push_back(Token(type, text, literal, line));
}

Scanner::Scanner(std::string source)
{
	this->source = source;
}

Scanner::Scanner(const Scanner &scanner)
{
	this->source = scanner.source;
	this->start = scanner.start;
	this->current = scanner.current;
	this->line = scanner.line;
	// this->tokens = scanner.tokens;
}
