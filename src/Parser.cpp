#include "Parser.hpp"
#include "Token.hpp"
#include <iostream>

bool Parser::httpMessage(void)
{
	if (!startLine())
		return (false);
	if (!crlf())
		return (false);
	if (!fieldLine())
		return (false);
	if (!crlf())
		return (false);
	if (!messageBody())
		return (false);
	if (!eof())
		return (false);
	if (current != (int)tokens.size())
		return (false);
	return (true);
}

bool Parser::startLine(void)
{
	if (!method())
		return (false);
	if (!space())
		return (false);
	if (!requestTarget())
		return (false);
	if (!space())
		return (false);
	if (!httpVersion())
		return (false);
	return (true);
}

bool Parser::method(void)
{
	if (tokens[current].getType() == Token::METHOD)
		current++;
	else
	{
		Parser::error(tokens[current].getLine(), 
		Token::tokenToString(tokens[current].getType()),
		Token::tokenToString(Token::METHOD));
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
		Token::tokenToString(tokens[current].getType()),
		Token::tokenToString(Token::SINGLE_SPACE));
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
		Token::tokenToString(tokens[current].getType()),
		Token::tokenToString(Token::URI));
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
		Token::tokenToString(tokens[current].getType()),
		Token::tokenToString(Token::VERSION));
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
		Token::tokenToString(tokens[current].getType()),
		Token::tokenToString(Token::LF));
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
		Token::tokenToString(tokens[current].getType()),
		Token::tokenToString(Token::BODY));
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
		Token::tokenToString(tokens[current].getType()),
		Token::tokenToString(Token::END_OF_FILE));
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




