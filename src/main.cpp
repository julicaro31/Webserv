#include <iostream>
#include "ParsingHelper.hpp"
#include "ConfigBlock.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"

int main(int ac, char *argv[])
{
	if (ac < 2)
	{
		std::cout << "Wrong number of arguments." << std::endl;
		return -1;
	}
	if (ac == 2)
	{
		std::cout << "Welcome to the webserv" << std::endl;

		// Parse and print the configuration file.
		try
		{
			std::string filePath(argv[1]);
			std::vector<ServerConfig> serversCongiguration = ParsingHelper::createServersConfiguration(filePath);
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
		}

		std::string line = "GET /path/to/resource HTTP/1.1\r\nHost: example.com\n\r\n";
		Request request = ParsingHelper::parseRequest(line);
	}
	else
	{
		std::cout << "<< DEV version >>" << std::endl;
		ServerConfig config = {
			false,
			true,
			1000000,
			8080,
			"localhost",
			"index.html",
			"/html",
			{{400, "error400.html"},
			 {403, "error403.html"},
			 {404, "error404.html"}}};

		ServerManager serverManager;
		serverManager.addServer(config);
		serverManager.printServers();
	}

	return 0;
}