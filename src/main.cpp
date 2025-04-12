#include <iostream>
#include "ParsingHelper.hpp"
#include "ConfigBlock.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"
#include "Response.hpp"

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
		try
		{
			std::string filePath(argv[1]);
			std::vector<ServerConfig> serversConfiguration = ParsingHelper::getServersConfig(filePath);
			ServerManager serverManager;
			for (std::vector<ServerConfig>::iterator it = serversConfiguration.begin(); it != serversConfiguration.end(); it++)
			{
				serverManager.addServer(*it);
			}
			// serverManager.printServers();

			if (serverManager.getServers().size() > 0)
			{
				// TEST RESPONSE WITH FIRST SERVER (example)
				testResponse("/index.html", *serverManager.getServers()[0]);
			}
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