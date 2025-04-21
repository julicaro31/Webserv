#include "Parser.hpp"
#include "Token.hpp"
#include <iostream>

bool Parser::httpMessage(void)
{
	return (startLine() &&
			crlf() &&
			fieldLine() &&
			crlf() &&
			messageBody() &&
			eof () &&
			(current == (int)tokens.size()));
}

bool Parser::startLine(void)
{
	return (method() &&
			space() &&
			requestTarget() &&
			space() &&
			httpVersion());
}

bool Parser::method(void)
{
	if (tokens[current].getType() == Token::METHOD)
		current++;
	else
	{
		Parser::error(tokens[current].getLine(), 
		Token::tokenTypeToString(tokens[current].getType()),
		Token::tokenTypeToString(Token::METHOD));
		return (false);
	}
	return (true);
}

bool Parser::space(void)
{
	if (tokens[current].getType() == Token::SINGLE_SPACE)
		current++;
	else
	{
		Parser::error(tokens[current].getLine(), 
		Token::tokenTypeToString(tokens[current].getType()),
		Token::tokenTypeToString(Token::SINGLE_SPACE));
		return (false);
	}
	return (true);
}

bool Parser::requestTarget(void)
{
	if (tokens[current].getType() == Token::URI)
		current++;
	else
	{
		Parser::error(tokens[current].getLine(), 
		Token::tokenTypeToString(tokens[current].getType()),
		Token::tokenTypeToString(Token::URI));
		return (false);
	}
	return (true);

}

bool Parser::httpVersion(void)
{
	if (tokens[current].getType() == Token::VERSION)
		current++;
	else
	{
		Parser::error(tokens[current].getLine(), 
		Token::tokenTypeToString(tokens[current].getType()),
		Token::tokenTypeToString(Token::VERSION));
		return (false);
	}
	return (true);
}

bool Parser::crlf(void)
{
	if (tokens[current].getType() == Token::LF || tokens[current].getType() == Token::CRLF)
		current++;
	else
	{
		Parser::error(tokens[current].getLine(), 
		Token::tokenTypeToString(tokens[current].getType()),
		Token::tokenTypeToString(Token::LF));
		return (false);
	}
	return (true);
}

bool Parser::fieldLine(void)
{
	while (tokens[current].getType() == Token::HEADER)
	{
		current++;
		if (!crlf())
			return (false);
	}
	return (true);
}

bool Parser::messageBody(void)
{
	if (tokens[current].getType() == Token::BODY)
		current++;
	else
	{
		Parser::error(tokens[current].getLine(), 
		Token::tokenTypeToString(tokens[current].getType()),
		Token::tokenTypeToString(Token::BODY));
		return (false);
	}
	return (true);
}

bool Parser::eof(void)
{
	if (tokens[current].getType() == Token::END_OF_FILE)
		current++;
	else
	{
		Parser::error(tokens[current].getLine(), 
		Token::tokenTypeToString(tokens[current].getType()),
		Token::tokenTypeToString(Token::END_OF_FILE));
		return (false);
	}
	return (true);
}

void Parser::error(int line, std::string found, std::string expected)
{
	std::cerr << "On line: " << line << " found \"" << found << "\" token, expected \"" << expected << "\"" << std::endl;
}

Parser::Parser(std::vector<Token> tokens)
: tokens{tokens}
{
}

Parser::Parser(const Parser &parser)
: tokens{parser.tokens}
, current{parser.current}
{
}

Parser::~Parser(void)
{
}




