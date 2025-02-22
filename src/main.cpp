#include <iostream>
#include "HttpParser.hpp"
#include "ParsingHelper.hpp"
#include "ConfigBlock.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"

int main(int ac, char *argv[])
{
	Logger::init("logs.log");
	if (ac < 2)
	{
		std::cout << "Wrong number of arguments." << std::endl;
		return -1;
	}
	if (ac == 2)
	{
		Logger::log(INFO, "Parsing configuration file...");
		// Parse and print the configuration file.
		std::string filePath(argv[1]);
		ConfigBlock configFile = ParsingHelper::parseConfigFile(filePath);
		configFile.print();

		std::string line = "GET /path/to/resource HTTP/1.1\r\nHost: example.com\n\r\n";
		Request request = HttpParser::parseRequest(line);
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
