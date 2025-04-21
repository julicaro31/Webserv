#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "Token.hpp"
#include <cctype>
#include <unordered_map>
#include <vector>

#define SPACE 13
#define	CR 32

class Scanner
{
public:
	std::unordered_map<int, std::vector<Token>> scanTokens(void);
	Scanner(std::string);
	Scanner(const Scanner &);
	~Scanner(void);
private:
	int	start = 0;
	int	current = 0;
	int	line = 1;
	const std::unordered_map<std::string, Token::TokenType> keywords = {
	{"GET", Token::TokenType::METHOD},
	{"POST", Token::TokenType::METHOD},
	{"DELETE", Token::TokenType::METHOD},
	{"PUT", Token::TokenType::METHOD},
	{"HEAD", Token::TokenType::METHOD},
	{"OPTIONS", Token::TokenType::METHOD},
	{"CONNECT", Token::TokenType::METHOD},
	{"TRACE", Token::TokenType::METHOD},
	{"PATCH", Token::TokenType::METHOD},
	};
	std::string source;
	std::unordered_map<int, std::vector<Token>>  mappedTokens;
private:
	void scanToken();
	void addToken(Token::TokenType);
	void addToken(Token::TokenType, std::string);
	void addToken(Token::TokenType, std::string, std::string);
	bool version();
	bool uri();
	bool header();
	void identifier();
	void body();
	char peek();
	char peek(int);
	char peekNext();
	bool isAtEnd(void);
	bool isAtEnd(int);
	char advance();
};

#endif
