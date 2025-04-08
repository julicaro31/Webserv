#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__

class ConfigBlock;

#include <string>
#include <cctype>
#include <unordered_map>
#include <vector>
#include "Scanner.hpp"
#include <ostream>
#include <iostream>

class HttpParser
{
public:
	static void parseRequest(std::string);
	static void error(int, std::string);
private:
	static std::vector<Token> vectorizeTokens(std::unordered_map<int, std::vector<Token>>);
	static	void report(int, std::string, std::string);
	HttpParser();
	HttpParser(const HttpParser &);
};

#endif
