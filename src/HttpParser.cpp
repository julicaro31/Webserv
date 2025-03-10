#include "HttpParser.hpp"
#include <vector>
#include "Token.hpp"


void HttpParser::parseRequest(std::string Request)
{
	Scanner scanner = Scanner(Request);
	std::vector<Token> tokens = scanner.scanTokens();

	for (auto token : tokens)
	{
		std::cout << token << std::endl;
	}
}

