#include "HttpParser.hpp"
#include <unordered_map>
#include <vector>
#include "Scanner.hpp"
#include <ostream>
#include <iostream>


void HttpParser::parseRequest(std::string request)
{
	Scanner scanner = Scanner(request);
	std::unordered_map<int, std::vector<Token>> tokens = scanner.scanTokens();

	// for (auto token : tokens)
	// {
	// 	std::cout << token << std::endl;
	// }
}

void HttpParser::error(int line, std::string message)
{
	report(line, "", message);
}

void HttpParser::report(int line, std::string location, std::string message)
{
	std::cout << "Line: " << line << " Error:" << location << ":" << message << std::endl;
}
