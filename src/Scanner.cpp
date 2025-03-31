#include "Scanner.hpp"
#include <cctype>
#include <string>
#include <vector>
#include "HttpParser.hpp"
#include <iostream>
#include "debug.hpp"

std::vector<Token> Scanner::scanTokens(void)
{
	std::cout << source;
	int i = 0;
	while (!isAtEnd())
	{
		start = current;
		scanToken();
		++i;
		std::cout << tokens.back();
		DEBUG_PRINT("i: " + std::to_string(i));
		DEBUG_PRINT("start: " + std::to_string(start));
		DEBUG_PRINT("current: " + std::to_string(current));
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
	char c = peek();
	DEBUG_PRINT("current token: " + std::to_string(c));
	DEBUG_PRINT("peek() == " + std::to_string(peek()));
	DEBUG_PRINT("int(peek()) == " + std::to_string(int(peek())));
	switch (c){
		case '*': DEBUG_PRINT("uri '*' case"); if (peek() == ' ') addToken(Token::URI);
		case '/': DEBUG_PRINT("uri '/' case"); uri();
		case ' ': 
			DEBUG_PRINT("space case");
			if (peekNext() == ' ')
			{
				addToken(Token::WHITESPACE);
				while (peek() == ' ') advance();
			}
			else
				addToken(Token::SINGLE_SPACE);
			break;
		case '\r': 
			DEBUG_PRINT("CR case");
			if (peekNext() == '\n')
			{
				addToken(Token::CRLF);
				line++;
				current += 2;
			}
			else
				addToken(Token::WHITESPACE); 
			break;
		case '\t': DEBUG_PRINT("tab case"); addToken(Token::WHITESPACE); advance(); break;
		case '\v': DEBUG_PRINT("vertical tab case"); addToken(Token::WHITESPACE); advance(); break;
		case '\f': DEBUG_PRINT("form feed case"); addToken(Token::WHITESPACE); advance(); break;
		case '\n': DEBUG_PRINT("LF case"); addToken(Token::LF); line++; break;
		case '"': DEBUG_PRINT("string case"); string();
		case 'H': DEBUG_PRINT("version case"); version();
		case 'h': DEBUG_PRINT("uri case"); uri(); 
	default:
		DEBUG_PRINT("default case");
		if (std::isdigit(c))
			number();
		else if (std::isalpha(c))
		{
			if (header()) break;
			else; identifier();
		}
		else
			HttpParser::error(line, "Unexpected character.");
	}
	if (start == current)
		advance();
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
	if (peek(8) == SPACE || peek(8) == CR);
	else return;
	current += 9;
	std::string value = source.substr(start, (current - 1) - start);
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
	DEBUG_PRINT("number case");
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
	DEBUG_PRINT("peek() == " + std::to_string(peek()));
	if (peek() == '/')
	{
		DEBUG_PRINT("in peek if statement, before while loop");
		while (peek(i) != ' ' && !isAtEnd(i))
		{
			if (peek(i) == '\n')
			{
				HttpParser::error(line, "Wrong URI origin-form format");
				return;
			}
			++i;
		}
		DEBUG_PRINT("in peek if statement, after while loop");
		if (isAtEnd(i))
		{
			HttpParser::error(line, "Wrong URI origin-form format");
			return;
		}
		std::string value = source.substr(start, (current + i) - start);
		addToken(Token::URI, value);
		current += i;
		DEBUG_PRINT("token added in peek == '\' ");
		return;
	}
	DEBUG_PRINT("before http check");
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
		current += i;
		DEBUG_PRINT("token added in http check");
	}
}

bool Scanner::header()
{
	int i = 0;
	int header_index = 0;
	DEBUG_PRINT("in header()");
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
	if (peek(i) == ':' && peek(i + 1) == CR)
		header_index = ++i;
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
	current += i;
	return (true);
}

void Scanner::identifier()
{
	DEBUG_PRINT("identifier case");
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
	std::string text = source.substr(start, current - start);
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
	// TODO: implement deep copy of tokens
	// this->tokens = scanner.tokens;
}
