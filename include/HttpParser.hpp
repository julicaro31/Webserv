#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__

class ConfigBlock;

#include <string>
#include <cctype>

class HttpParser
{
public:
	static void parseRequest(std::string);
	static bool hadError;
	void error(int, std::string);
private:
	static	void report(int, std::string, std::string);
	HttpParser();
	HttpParser(const HttpParser &);
};

#endif
