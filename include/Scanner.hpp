#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "Token.hpp"
#include <cctype>
#include <vector>
		

#define SPACE 13
#define	CR 32

class Scanner
{
private:
	int	start = 0;
	int	current = 0;
	int	line = 1;
	std::unordered_map<std::string, Token::TokenType> keywords = {
	{"GET", Token::TokenType::GET},
	{"POST", Token::TokenType::POST},
	{"DELETE", Token::TokenType::DELETE},
	{"PUT", Token::TokenType::PUT},
	{"HEAD", Token::TokenType::HEAD},
	{"OPTIONS", Token::TokenType::OPTIONS},
	{"CONNECT", Token::TokenType::CONNECT},
	{"TRACE", Token::TokenType::TRACE},
	{"PATCH", Token::TokenType::PATCH},
	};
	std::string source;
	std::vector<Token> tokens;
	bool isAtEnd(void);
	bool isAtEnd(int);
	void scanToken();
	char advance();
	bool match(char);
	void string();
	void version();
	void number();
	void uri();
	bool header();
	void identifier();
	char peek();
	char peek(int);
	char peekNext();
	void addToken(Token::TokenType);
	void addToken(Token::TokenType, std::string);
public:
	std::vector<Token> scanTokens(void);
	Scanner(std::string);
	Scanner(const Scanner &);
};

#endif
