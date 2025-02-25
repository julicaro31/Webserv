#ifndef __HTTP_PARSER_H__
#define __HTTP_PARSER_H__

class ConfigBlock;

#include "Directives.hpp"
#include "Request.hpp"
#include "Method.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

class HttpParser
{
public:
	static void parseRequest(std::string strRequest);

private:
	HttpParser();
	HttpParser(const HttpParser &parser);
};

#endif
