#include "HttpParser.hpp"
#include "Request.hpp"
#include <unordered_map>
#include <Parser.hpp>


void HttpParser::parseRequest(std::string request)
{
	Scanner scanner = Scanner(request);
	std::unordered_map<int, std::vector<Token>> mappedTokens = scanner.scanTokens();
	std::vector<Token> tokens = HttpParser::vectorizeTokens(mappedTokens);
	// for (auto it:tokens)
	// {
	// 	std::cout << it;
	// }
	Parser parser = Parser(tokens);
	if (parser.httpMessage())
		std::cout << "correct message" << std::endl;
	else
		std::cout << "wrongly formatted message" << std::endl;
	Request reqst = Request(tokens);
	std::cout << reqst;
}

std::vector<Token> HttpParser::vectorizeTokens(std::unordered_map<int, std::vector<Token>> tokens)
{
	int size = tokens.size();
	std::vector<Token> tokenVector;

	for (int i = 0; i <= size; ++i)
	{
		std::vector<Token> temp = tokens[i];
		for (auto it:temp)
		{
			tokenVector.push_back(it);
		}
	}
	return (tokenVector);
}

void HttpParser::error(int line, std::string message)
{
	report(line, "", message);
}

void HttpParser::report(int line, std::string location, std::string message)
{
	std::cerr << "Line: " << line << " Error:" << location << ":" << message << std::endl;
}
