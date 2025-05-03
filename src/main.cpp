#include <iostream>
#include "HttpParser.hpp"
#include "ParsingHelper.hpp"
#include "ConfigBlock.hpp"
#include "Server.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"
#include "Request.hpp"
#include "debug.hpp"
#include "Response.hpp"

int main(int ac, char *argv[])
{
	std::vector<Token> tokens;
	Logger::init("logs.log");
	if (ac < 2)
	{
		std::cout << "Wrong number of arguments. Too few arguments" << std::endl;
		return -1;
	}
	else if (ac == 3)
	{
		std::vector<ServerConfig> serversConfiguration;

		std::cout << "<< DEV version >>" << std::endl;
		Logger::log(INFO, "Starting server...");
		Logger::log(INFO, "Setting up configuration");

		std::string filePath(argv[2]);
		try {
			serversConfiguration = ParsingHelper::getServersConfig(filePath);
		} catch (const std::exception &e) {
			std::cerr << "failed to parse due to " << e.what() << std::endl;
		}
		ServerManager serverManager;
		for (std::vector<ServerConfig>::iterator it = serversConfiguration.begin(); it != serversConfiguration.end(); it++)
		{
			serverManager.addServer(*it);
		}
		
		Logger::log(INFO, "finished configuration");
		if (serverManager.getServers().size() > 0)
		{
			if ((argv[1]) == std::string("start"))
			{
				Logger::log(INFO, "serverManager setting up poll");
				serverManager.runPoll();
			}
			else
			{
				std::cerr << "Wrong type of arguments." << std::endl;
				return -1;
			}
		}
		else
		{
			std::cerr << "configuration failed" << std::endl;
			return (-1);
		}
	}
	else
	{
		std::cerr << "Wrong number of arguments. Too many arguments" << std::endl;
		return -1;
	}
	return 0;
}
