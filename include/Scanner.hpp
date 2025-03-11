#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "Token.hpp"
#include <cctype>
#include <vector>

class Scanner
{
private:
	int	start = 0;
	int	current = 0;
	int	line = 1;
	std::string source;
	std::vector<Token> tokens;
	bool isAtEnd(void);
	void scanToken();
	char advance();
	void addToken(TokenType);
	void addToken(TokenType, std::string);
public:
	std::vector<Token> scanTokens(void);
	Scanner(std::string);
	Scanner(const Scanner &);
};

#endif
