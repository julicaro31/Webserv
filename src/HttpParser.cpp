#include "HttpParser.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include <unordered_map>
#include <Parser.hpp>
#include <vector>
#include "debug.hpp"

std::vector<Token> HttpParser::parseRequest(std::string request)
{
	Scanner scanner = Scanner(request);
	std::unordered_map<int, std::vector<Token>> mappedTokens = scanner.scanTokens();
	std::vector<Token> tokens = HttpParser::vectorizeTokens(mappedTokens);
	Parser parser = Parser(tokens);
	for (auto token:tokens)
	{
		std::cout << token.toString() << std::endl;
	}
	if (parser.httpMessage())
		Logger::log(INFO, "correct HTTP request received");
	else
	{ 
		Logger::log(ERROR, "wrongly formatted HTTP request received");
		throw "HTTP Request not correctly formated";
	}
	return (tokens);
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
	Logger::log(ERROR, "Line: " + std::to_string(line) + " Error: " + location + ":" + message);
}
