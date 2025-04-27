#include <iostream>
#include "HttpParser.hpp"
#include "ParsingHelper.hpp"
#include "ConfigBlock.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "debug.hpp"

int main(int ac, char *argv[])
{
	std::vector<Token> tokens;
	Logger::init("logs.log");
	if (ac < 2)
	{
		std::cout << "Wrong number of arguments." << std::endl;
		return -1;
	}
	if (ac == 2)
	{
		Logger::log(INFO, "Parsing configuration file...");
		std::string line = "POST /api/login HTTP/1.1\r\nHost: example.com\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html\r\n\r\n{ \"username\": \"user123\", \"password\": \"pass123\"}\r\n";
		try
		{
			tokens = HttpParser::parseRequest(line);
		}
		catch (const char *exception)
		{
			std::cerr << "Error: " << exception << std::endl;
		}
		catch (...)
		{
			std::cerr << "Unknown error" << std::endl;
		}
		Request reqst = Request(tokens);
		DEBUG_PRINT(Request::requestToString(reqst));
		try
		{
			std::string filePath(argv[1]);
			std::vector<ServerConfig> serversConfiguration = ParsingHelper::getServersConfig(filePath);
			ServerManager serverManager;
			for (std::vector<ServerConfig>::iterator it = serversConfiguration.begin(); it != serversConfiguration.end(); it++)
			{
				serverManager.addServer(*it);
			}
			serverManager.printServers();
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	else if (ac == 3)
	{
		std::ifstream file(argv[2]);
		if (!file.is_open())
		{
			std::cerr << "Error opening file: " << argv[2] << std::endl;
			return -1;
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		HttpParser::parseRequest(content);
	}
	else
	{
		std::cout << "<< DEV version >>" << std::endl;
		Logger::log(INFO, "Starting server...");
		std::vector<Location> locations;
		ServerConfig config = {
			8080,
			false,
			true,
			1000000,
			"localhost",
			"/html",
			"name",
			{302, "http.."},
			{{400, "error400.html"},
			 {403, "error403.html"},
			 {501, "error50x.html"},
			 {502, "error50x.html"}},
			{"index.html"},
			locations};

		ServerManager serverManager;
		serverManager.addServer(config);
		serverManager.printServers();
		if ((argv[1]) == std::string("start"))
		{
			serverManager.runPoll();
		}
	}

	return 0;
}