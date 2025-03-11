#include "Scanner.hpp"
#include <vector>
#include "Token.hpp"

Scanner::std::vector<Token> Scanner::scanTokens(void)
{
	std::vector<Token> tokens;
	while (!isAtEnd())
	{
		start = current;
		scanToken();
	}
	tokens.push_back(Token(EOF, "", nullptr, line));
	return (tokens);
}

Scanner::isAtEnd(void)
{
	return (current >= source.length());
}

Scanner::Scanner(std::string source)
{
	this->source = source;
}

Scanner::Scanner(const Scanner &scanner)
{
}
