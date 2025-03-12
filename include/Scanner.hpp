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
	bool match(char);
	void string();
	void number();
	void identifier();
	char peek();
	char peekNext();
	void addToken(Token::TokenType);
	void addToken(Token::TokenType, std::string);
public:
	std::vector<Token> scanTokens(void);
	Scanner(std::string);
	Scanner(const Scanner &);
};

const std::unordered_map<std::string, Token::TokenType> Token::keywords = {
    {"where",    Token::TokenType::WHERE},
};

#endif
