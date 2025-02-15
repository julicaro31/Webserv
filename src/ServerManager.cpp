#include "ServerManager.hpp"
#include <algorithm>

ServerManager::ServerManager()
{
}

ServerManager::ServerManager(const ServerManager &other)
{
	*this = other;
}

ServerManager &ServerManager::operator=(const ServerManager &other)
{
	if (this != &other)
	{
		_servers = other._servers;
		_pollFDs = other._pollFDs;
		_serverFDs = other._serverFDs;
		_clientActivity = other._clientActivity;
	}
	return *this;
}

ServerManager::~ServerManager()
{
}

void ServerManager::addServer(const Server &server)
{
	_servers.push_back(server);
}

// not sure if needed!
void ServerManager::removeServer(const Server &server)
{
	_servers.erase(std::remove(_servers.begin(), _servers.end(), server), _servers.end());
}
const std::vector<Server> &ServerManager::getAllServers() const
{
	return _servers;
}

void ServerManager::printServers() const
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "Server " << i << ":\n";
		std::cout << "Host: " << _servers[i].getHost() << std::endl;
		std::cout << "Port: " << _servers[i].getPort() << std::endl;
		std::cout << "Root: " << _servers[i].getRoot() << std::endl;
	}
}

//Check if host:port is already in use in a map<std::string, int> used_ports.