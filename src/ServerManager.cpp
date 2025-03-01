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
	std::unique_ptr<Server> server = std::make_unique<Server>();

	server->setHost(config.host);
	server->setPort(config.port);
	server->setRoot(config.root);
	server->setIndex(config.index);
	server->setAutoIndex(config.autoIndex);
	server->setMaxBodySize(config.maxBodySize);
	server->setErrorPages(config.errorPages);

	if (server->setupSocket())
	{
		int serverFD = server->getSocketFD();
		// Add the server's file descriptor to the poll list and move it to _servers
		_pollFDs.push_back({serverFD, POLLIN, 0});
		_servers.push_back(std::move(server));
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

// for testing purposes
void ServerManager::printServers() const
{
	std::cout << "ServerManager contains " << _servers.size() << " servers." << std::endl;
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "Server " << i << ":\n";
		std::cout << "Host: " << _servers[i]->getHost() << std::endl;
		std::cout << "Port: " << _servers[i]->getPort() << std::endl;
		std::cout << "Root: " << _servers[i]->getRoot() << std::endl;
		std::cout << "Index: ";
		for (const std::string &element : _servers[i]->getIndex())
		{
			std::cout << element << " ";
		}
		std::cout << std::endl;
		std::cout << "AutoIndex: " << _servers[i]->isAutoIndexEnabled() << std::endl;
		std::cout << "MaxBodySize: " << _servers[i]->getMaxBodySize() << std::endl;
		std::cout << "Error pages:\n";
		for (const auto &pair : _servers[i]->getErrorPages())
		{
			std::cout << "- " << pair.first << " : ";

			for (const auto &val : pair.second)
			{
				std::cout << val << " ";
			}

			std::cout << std::endl;
		}
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
	for (const auto &server : _servers)
	{
		if (server->getSocketFD() == fd)
			return server.get();
	}
	return nullptr;
}

/**
 * @brief Run the poll loop to handle incoming connections and client requests.
 *
 */
void ServerManager::runPoll()
{
	std::cout << "[ServerManager] Running " << _servers.size() << " servers..." << std::endl;
	while (true)
	{
		checkTimeouts();
		// Call poll() with a timeout of 500 milliseconds
		int ready = poll(_pollFDs.data(), _pollFDs.size(), 500);
		if (ready < 0)
		{
			perror("poll error");
			// need to close all sockets
			continue;
		}

		// Loop through poll_fds to handle each socket
		for (size_t i = 0; i < _pollFDs.size(); i++)
		{
			// Check if there's an event on this socket
			if (_pollFDs[i].revents & POLLIN)
			{
				// Check if it's a server FD (new connection)
				if (getServerByFileDescriptor(_pollFDs[i].fd) != nullptr)
				{
					acceptNewClient(_pollFDs[i].fd);
				}
				else
				{
					// If it's a client socket, read data
					handleClientRequest(_pollFDs[i].fd);
				}
			}
			if (_pollFDs[i].revents & POLLHUP && !(_pollFDs[i].revents & POLLIN))
			{
				std::cout << "[ServerManager] Detected POLLHUP on FD " << _pollFDs[i].fd << std::endl;
				removeClient(_pollFDs[i].fd);
			}
		}
	}
}

void ServerManager::acceptNewClient(int serverFD)
{
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);
	int newClientFD = accept(serverFD, (struct sockaddr *)&address, &addrlen);
	if (newClientFD < 0)
	{
		perror("Failed to accept new client(connection)");
		return;
	}
	std::cout << "[ServerManager] New client connected on FD " << newClientFD << std::endl;

	// Add new client socket to poll list, and map client FD to server
	pollfd pfd;
	pfd.fd = newClientFD;
	pfd.events = POLLIN | POLLOUT;
	_pollFDs.push_back(pfd);
	_clientToServer[newClientFD] = getServerByFileDescriptor(serverFD);
	_clientActivity[newClientFD] = time(NULL); // save client activity time
}

void ServerManager::handleClientRequest(int clientFD)
{
	char buffer[1024];
	int r = read(clientFD, buffer, sizeof(buffer) - 1);

	if (r < 0)
	{
		perror("Failed to read from client");
		removeClient(clientFD);
		return;
	}
	else if (r == 0)
	{
		removeClient(clientFD);
		return;
	}
	buffer[r] = '\0';

	_clientActivity[clientFD] = time(NULL); // Update activity timestamp

	std::cout << "[ServerManager] Received from client FD " << clientFD << '\n'
			  << buffer << std::endl;

	// Respond with a simple "Hello, World!" message
	const char *body = "Hello, World!";
	std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(strlen(body)) + "\r\n" + "Connection: keep-alive" + "\r\n\r\n" + body;
	if (send(clientFD, response.c_str(), response.length(), 0) < 0)
	{
		perror("[ServerManager] Failed to send response");
		removeClient(clientFD);
	}
}

void ServerManager::removeClient(int clientFD)
{
	close(clientFD);
	for (auto it = _pollFDs.begin(); it != _pollFDs.end(); ++it)
	{
		if (it->fd == clientFD)
		{
			_pollFDs.erase(it);
			break;
		}
	}
	_clientToServer.erase(clientFD);
	_clientActivity.erase(clientFD);
	std::cout << "[ServerManager] Client FD " << clientFD << " disconnected." << std::endl;
}

// Check for inactive clients and remove them from the poll list
void ServerManager::checkTimeouts()
{
	time_t now = time(NULL);
	for (size_t i = 0; i < _pollFDs.size(); i++)
	{
		int clientFD = _pollFDs[i].fd;
		if (_clientActivity.find(clientFD) != _clientActivity.end() &&
			now - _clientActivity[clientFD] > CLIENT_TIMEOUT)
		{
			std::cout << "[ServerManager] Client FD " << clientFD << " timed out." << std::endl;
			removeClient(clientFD);
		}
	}
}
