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
		case '.': if (peek() == ' ') addToken(DOT); break;
		case ':': if (peek() == ' ') addToken(COLON); break;
		case '*': if (peek() == ' ') addToken(STAR); break;
		case '?': if (peek() == ' ') addToken(QUESTION_MARK); break;
		case '=': if (peek() == ' ') addToken(EQUAL); break;
		case '!': if (peek() == ' ') addToken(match('=') ? TOKEN4 : TOKEN5);
		case '/': 
			if (match('/'))
				addToken(DOUBLE_SLASH);
			else 
				addToken(SLASH);
			break;
		case ' ': 
			if (match(' '))
				addToken(WHITESPACE)
			else
				addToken(Token::SINGLE_SPACE);
			while (peek() != ' ') advance();
			break;
		case '\r': 
			if (match('\n'))
			{
				addToken(CRLF);
				line++;
			}
			else
				addToken(WHITESPACE);
			break;
		case '\t': addToken(WHITESPACE) break;
		case '\n': line++; break;
		case '"': string(); break;
		case 'H': version(); break;
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

void Scanner::version()
{
	int start = current;

    if (peek() != 'H' || peek(1) != 'T' || peek(2) != 'T' || peek(3) != 'P')
        return false;
    if (peek(4) != '/') return false;
    if (!std::isdigit(peek(5))) return false;
    if (peek(6) != '.') return false;
    if (!std::isdigit(peek(7))) return false;
    if (peek(8) != ' ') return false;
    current += 9;
	std::string value = source.subtstring(start + 1, current - 1);
	addToken(VERSION, value);
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
