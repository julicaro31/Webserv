#include <memory>
#include "ServerManager.hpp"
#include <algorithm>

ServerManager::ServerManager()
{
}

ServerManager::~ServerManager()
{
}

/**
 * @brief  Add a server to the server manager
 *
 * @param config struct containing the server configuration
 */
void ServerManager::addServer(const ServerConfig &config)
{
	Server server;

	server.setHost(config.host);
	server.setPort(config.port);
	server.setRoot(config.root);
	server.setIndex(config.index);
	server.setAutoIndex(config.autoIndex);
	server.setMaxBodySize(config.maxBodySize);
	server.setErrorPages(config.errorPages);

	if (server.setupSocket())
	{
		_servers.push_back(std::make_unique<Server>(std::move(server)));
		std::cout << "[ServerManager] Server added: " << config.host << ":" << config.port << std::endl;
	}
	else
	{
		std::cerr << "[ServerManager] Error setting up server: " << config.host << ":" << config.port << std::endl;
	}
}

const std::vector<std::unique_ptr<Server>> &ServerManager::getServers() const
{
	return _servers;
}

void ServerManager::printServers() const
{
	std::cout << "ServerManager contains " << _servers.size() << " servers." << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "Server " << i << ":\n";
		std::cout << "Host: " << _servers[i]->getHost() << std::endl;
		std::cout << "Port: " << _servers[i]->getPort() << std::endl;
		std::cout << "Root: " << _servers[i]->getRoot() << std::endl;
	}
}

// Check if host:port is already in use in a map<std::string, int> used_ports.


/**
 * @brief Get the Server object(raw pointer) by file descriptor
 * @param fd 
 * @return Server* 
 */
Server *ServerManager::getServerByFileDescriptor(int fd) const
{
	for (const auto &server: _servers)
	{
		if (server->getSocketFD() == fd)
			return server.get();
	}
	return nullptr;
}