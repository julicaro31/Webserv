#include <iostream>
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
		std::cout << "Welcome to the webserv" << std::endl;

		// Parse and print the configuration file.
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
		ServerConfig config = {
			false,
			true,
			1000000,
			8080,
			"localhost",
			{"index.html"},
			"/html",
			"name",
			{302, "http.."},
			{{"error400.html", {400}},
			 {"error403.html", {403}},
			 {"error50x.html", {501, 502, 503}}}};

		ServerConfig config2 = {
			false,
			true,
			1000000,
			5000,
			"127.0.0.2",
			"index.html",
			"/html",
			{{400, "error400.html"},
			 {403, "error403.html"},
			 {404, "error404.html"}}};

		ServerManager serverManager;
		serverManager.addServer(config);
		serverManager.addServer(config2);
		serverManager.printServers();
		if ((argv[1]) == std::string("start"))
		{
			serverManager.runPoll();
		}
	}

	return 0;
}