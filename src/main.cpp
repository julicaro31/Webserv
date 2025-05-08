#include "ServerManager.hpp"
#include "debug.hpp"

int main(int ac, char *argv[])
{
	Logger::init("logs.log");
	if (ac > 2)
	{
		std::cout << "Wrong number of arguments. Too many arguments" << std::endl;
		return -1;
	}

	std::string filePath = ac == 1 ? "config_files/default.conf" : argv[1];

	std::vector<ServerConfig> serversConfiguration;
	Logger::log(INFO, "Starting server...");
	Logger::log(INFO, "Setting up configuration");
	try
	{
		serversConfiguration = ParsingHelper::getServersConfig(filePath);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return -1;
	}

	ServerManager serverManager;
	for (std::vector<ServerConfig>::iterator it = serversConfiguration.begin(); it != serversConfiguration.end(); it++)
	{
		serverManager.addServer(*it);
	}
	Logger::log(INFO, "Finished configuration");

	std::cout << "Welcome to webserv!" << std::endl;
	if (serverManager.getServers().size() > 0)
	{
		Logger::log(INFO, "serverManager setting up poll");
		serverManager.runPoll();
		serverManager.closeFDs();
	}
	else
	{
		std::cerr << "Configuration failed" << std::endl;
		return -1;
	}
	return 0;
}
