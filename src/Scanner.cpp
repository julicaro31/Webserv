#include "Scanner.hpp"
#include <cctype>
#include <string>
#include <vector>
#include "HttpParser.hpp"
#include <iostream>
#include "Logger.hpp"
#include "Token.hpp"
#include "debug.hpp"

std::unordered_map<int, std::vector<Token>> Scanner::scanTokens(void)
{
	DEBUG_PRINT("\nsource: \n" + source);
	int i = 0;
	Logger::log(INFO, "Start scanning http request.... ");
	while (!isAtEnd())
	{
		start = current;
		scanToken();
		++i;
		DEBUG_PRINT("i: " + std::to_string(i));
		DEBUG_PRINT("start: " + std::to_string(start));
		DEBUG_PRINT("current: " + std::to_string(current));
		std::vector<Token> last = mappedTokens[line];
		if (!last.empty())
			DEBUG_PRINT(Token::tokenToString(last.back()));
		else 
			DEBUG_PRINT("\n\nlast token is empty!\n\n");
	}
	start = current;
	addToken(Token::END_OF_FILE, "", "");
	Logger::log(INFO, "Finished scanning http request.... ");
	return (mappedTokens);
}


bool Scanner::isBody()
{
	std::vector<Token> previousLine;
	std::vector<Token> penultimateLine;
	Token::TokenType previousType;
	Token::TokenType penultimateType;
	int previousLineSize;

	DEBUG_PRINT("in isBody()");
	if (line == 2)
	{
		DEBUG_PRINT("in isBody() line == 2");
		previousLine = mappedTokens[line - 1];
		previousType = previousLine.back().getType();
		if ((previousType == Token::LF || previousType == Token::CRLF))
			return (true);
	}
	if (line > 2)
	{
		DEBUG_PRINT("in isBody() line > 2");
		previousLine = mappedTokens[line - 1];
		penultimateLine = mappedTokens[line - 2];
		previousLineSize = previousLine.size();

		previousType = previousLine.back().getType();
		penultimateType = penultimateLine.back().getType();
		if (previousLineSize == 1 && 
			(previousType == Token::LF || previousType == Token::CRLF) &&
			(penultimateType == Token::LF || penultimateType == Token::CRLF))
			return (true);
	}
	DEBUG_PRINT("isBody() == false");
	return (false);
}

void Scanner::scanToken()
{
	char c = peek();

	DEBUG_PRINT("peek() == " + std::string(1, peek()));
	DEBUG_PRINT("int(peek()) == " + std::to_string(int(peek())));
	switch (c){
		case '*': 
			DEBUG_PRINT("uri '*' case"); 
			if (peek() == ' ')
			{
				addToken(Token::URI);
				break;
			} 
			[[fallthrough]];
		case '/': 
			DEBUG_PRINT("uri '/' case"); 
			if (uri()) 
				break; 
			[[fallthrough]];
		case ' ': 
			DEBUG_PRINT("space case");
			if (peekNext() == ' ')
			{
				addToken(Token::WHITESPACE);
				while (peek() == ' ') advance();
			}
			else
			{
				addToken(Token::SINGLE_SPACE); 
				advance();
			}
			break;
		case '\r': 
			DEBUG_PRINT("CR case");
			advance();
			if (peek() == '\n')
			{
				addToken(Token::CRLF);
				advance();
				line++;
			}
			else
				addToken(Token::WHITESPACE);
			break;
		case '\t': 
			DEBUG_PRINT("tab case"); 
			addToken(Token::WHITESPACE); 
			advance(); 
			break;
		case '\v': 
			DEBUG_PRINT("vertical tab case"); 
			addToken(Token::WHITESPACE); 
			advance(); 
			break;
		case '\f': 
			DEBUG_PRINT("form feed case"); 
			addToken(Token::WHITESPACE); 
			advance(); 
			break;
		case '\n': 
			DEBUG_PRINT("LF case"); 
			addToken(Token::LF); 
			advance(); 
			line++; 
			DEBUG_PRINT("LF after case"); 
			break;
		case 'H': 
			DEBUG_PRINT("version case"); 
			if (version()) 
				break; 
			[[fallthrough]];
		case 'h': 
			DEBUG_PRINT("uri case"); 
			if (uri()) 
				break; 
			[[fallthrough]];
	default:
	DEBUG_PRINT("default case");
		if (header()) 
			break;
		else if (isBody())
			body();
		else
			identifier();
	}
	if (start == current)
		advance();
}

void Scanner::addToken(Token::TokenType type)
{
	addToken(type, "");
}

void Scanner::addToken(Token::TokenType type, std::string literal)
{
	std::string text = source.substr(start, current - start);
	mappedTokens[line].push_back(Token(type, text, literal, line));
}

void Scanner::addToken(Token::TokenType type, std::string text, std::string literal)
{
	mappedTokens[line].push_back(Token(type, text, literal, line));
}

bool Scanner::version()
{
	int start = current;

	if (peek() != 'H' || peek(1) != 'T' || peek(2) != 'T' || peek(3) != 'P')
		return (false);
	if (peek(4) != '/') 
		return (false);
	if (!std::isdigit(peek(5))) 
		return (false);
	if (peek(6) != '.') 
		return (false);
	if (!std::isdigit(peek(7))) 
		return (false);
	current += 8;
	std::string value = source.substr(start + 5, 3);
	addToken(Token::VERSION, value);
	return (true);
}

bool Scanner::uri()
{
	int i = 0;
	DEBUG_PRINT("peek() == " + std::to_string(peek()));
	if (peek() == '/')
	{
		DEBUG_PRINT("in peek if statement, before while loop");
		while (peek(i) != ' ' && !isAtEnd(i))
		{
			if (peek(i) == '\n')
				return (false);
			++i;
		}
		DEBUG_PRINT("in peek if statement, after while loop");
		if (isAtEnd(i))
			return (false);
		std::string value = source.substr(start, (current + i) - start);
		addToken(Token::URI, value);
		current += i;
		DEBUG_PRINT("token added in peek == '\' ");
		return (true);
	}
	DEBUG_PRINT("before http check");
	if (peek() == 'h' && peek(1) == 't' && peek(2) == 't' && peek(3) == 'p')
	{
		if (peek(4) != '/') return (false);
		if (peek(5) != '/') return (false);
		i = 5;
		while (peek(i) != ' ' && !isAtEnd(i))
		{
			if (peek(i) == '\n')
				return (false);
			++i;
		}
		if (isAtEnd(i))
			return (false);
		std::string value = source.substr(start, (current + i) - start);
		addToken(Token::URI, value);
		current += i;
		DEBUG_PRINT("token added in http check");
	}
	else
		return (false);
	return (true);
}

bool Scanner::header()
{
	int i = 0;
	DEBUG_PRINT("in header()");

	if (!std::isalpha(peek()))
		return (false);
	while (peek(i) != ':' && !isAtEnd(i))
	{
		if (peek(i) == '\n')
			return (false);
		++i;
	}
	if (isAtEnd(i))
		return (false);
	DEBUG_PRINT("before header delim");
	if (peek(i) != ':' || peek(i + 1) != CR)
		return (false);
	else ++i;
	while (peek(i) != '\n' && peek(i) != '\r' && !isAtEnd(i))
		++i;
	if (isAtEnd(i))
		return (false);
	std::string header = source.substr(start, (current + i) - start);
	addToken(Token::HEADER, header);
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

void Scanner::body()
{
	int i = current;

	while (peek(i))
		i++;
	std::string body = source.substr(start, (current + i) - start);
	addToken(Token::BODY, body);
	current += i;
}

char Scanner::peek()
{
	if (isAtEnd())
		return ('\0');
	return (source[current]);
}

char Scanner::peek(int index)
{
	if (isAtEnd(index))
		return ('\0');
	return (source[current + index]);
}

char Scanner::peekNext()
{
	if (current + 1 >= static_cast<int>(source.length()))
		return ('\0');
	return (source[current + 1]);
}

bool Scanner::isAtEnd(void)
{
	return (current >= static_cast<int>(source.length()));
}

bool Scanner::isAtEnd(int index)
{
	return (current + index >= static_cast<int>(source.length()));
}

char Scanner::advance()
{
	return (source[current++]);
}

Scanner::Scanner(std::string source)
	: source {source}
{
}

Scanner::Scanner(const Scanner &scanner)
	: start {scanner.start}
	, current {scanner.current}
	, line {scanner.line}
	, source {scanner.source}
	, mappedTokens {scanner.mappedTokens}
{
}

Scanner::~Scanner(void)
{
}
