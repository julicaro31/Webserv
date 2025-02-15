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
		std::string filePath(argv[1]);
		ConfigBlock configFile = ParsingHelper::parseConfigFile(filePath);
		configFile.print();

		std::string line = "GET /path/to/resource HTTP/1.1\r\nHost: example.com\n\r\n";
		Request request = ParsingHelper::parseRequest(line);
	}
	else
	{
		std::cout << "<< DEV version >>" << std::endl;
		Server server;
		server.setHost("localhost");
		server.setPort(8080);
		server.setRoot("/html");
		server.setIndex("index.html");
		server.setAutoIndex(false);
		server.setMaxBodySize(1000000);
		std::map<int, std::string> errorPages;
		errorPages[400] = "error400.html";
		errorPages[403] = "error403.html";
		errorPages[404] = "error404.html";
		server.setErrorPages(errorPages);

		ServerManager serverManager;
		serverManager.addServer(server);
		serverManager.printServers();

	}

	return 0;
}