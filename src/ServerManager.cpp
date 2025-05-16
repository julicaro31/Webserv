#include <memory>
#include "Response.hpp"
#include "HttpParser.hpp"
#include <Token.hpp>
#include "ServerManager.hpp"
#include <algorithm>
#include "Logger.hpp"
#include "Request.hpp"

volatile sig_atomic_t g_terminate = 0;

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
	server->setAutoIndex(config.autoIndex);
	server->setDefaultServer(config.defaultServer);
	server->setMaxBodySize(config.maxBodySize);
	server->setRoot(config.root);
	server->setServerName(config.serverName);
	server->setRedirection(config.redirection);
	server->setErrorPages(config.errorPages);
	server->setIndex(config.index);
	server->setLocations(config.locations);

	if (server->setupSocket())
	{
		int serverFD = server->getSocketFD();
		// Add the server's file descriptor to the poll list and move it to _servers
		_pollFDs.push_back({serverFD, POLLIN, 0});
		_servers.push_back(std::move(server));
		Logger::log(INFO, "[ServerManager] Server added: " + config.host + ":" + std::to_string(config.port));
	}
	else
	{
		Logger::log(ERROR, "[ServerManager] Error setting up server: " + config.host + ":" + std::to_string(config.port));
	}
}

const std::vector<std::unique_ptr<Server>> &ServerManager::getServers() const
{
	return _servers;
}

// for testing purposes, no need to move it to logger
void ServerManager::printServers() const
{
	std::cout << "ServerManager contains " << _servers.size() << " servers." << std::endl;
	Logger::log(INFO, "[ServerManager] ServerManager contains " + std::to_string(_servers.size()) + " servers.");
	for (size_t i = 0; i < _servers.size(); i++)
	{
		std::cout << "Server #" << i + 1 << " Name: " << _servers[i]->getServerName() << ":" << std::endl;
		std::cout << "Host: " << _servers[i]->getHost() << std::endl;
		std::cout << "Port: " << _servers[i]->getPort() << std::endl;
		std::cout << "AutoIndex: " << _servers[i]->isAutoIndexEnabled() << std::endl;
		std::cout << "Is default server: " << _servers[i]->isDefaultServer() << std::endl;
		std::cout << "MaxBodySize: " << _servers[i]->getMaxBodySize() << std::endl;
		std::cout << "Root: " << _servers[i]->getRoot() << std::endl;

		std::cout << "Redirection: " << _servers[i]->getRedirection().first << " " << _servers[i]->getRedirection().second << std::endl;

		std::cout << "Error pages:" << std::endl;
		for (const std::pair<const int, std::string> &errorPage : _servers[i]->getErrorPages())
		{
			std::cout << "-" << errorPage.first << ": " << errorPage.second << std::endl;
		}

		std::cout << "Index: ";
		for (const std::string &element : _servers[i]->getIndex())
		{
			std::cout << element << " ";
		}
		std::cout << std::endl;

		printLocations(_servers[i]->getLocations());

		std::cout << "===========================================" << std::endl;
	}
}

void ServerManager::printLocations(std::vector<Location> locations) const
{
	for (std::vector<Location>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		std::cout << std::endl;
		std::cout << "Location " << it->modifier << " " << it->uri << std::endl;
		std::cout << "	AutoIndex: " << it->autoIndex << std::endl;
		std::cout << "	MaxBodySize: " << it->maxBodySize << std::endl;
		std::cout << "	Root: " << it->root << std::endl;

		std::cout << "	Redirection: " << it->redirection.first << " " << it->redirection.second << std::endl;

		std::cout << "	Error pages:" << std::endl;
		for (const std::pair<const int, std::string> &errorPage : it->errorPages)
		{
			std::cout << "	-" << errorPage.first << ": " << errorPage.second << std::endl;
		}

		std::cout << "	CGI: " << it->cgiPass << std::endl;

		std::cout << "	Index: ";
		for (const std::string &element : it->index)
		{
			std::cout << "	" << element << " ";
		}
		std::cout << std::endl;

		std::cout << "	Limit except: " << std::endl;
		for (std::vector<LimitExceptDirective>::iterator led = it->limitExcepts.begin(); led != it->limitExcepts.end(); led++)
		{
			std::cout << "	-Mehods: ";
			for (std::vector<Method>::iterator method = led->methods.begin(); method != led->methods.end(); method++)
			{
				std::cout << ParsingHelper::methodToStr(*method) << " ";
			}

			std::cout << " Allow: " << led->allow << " Deny: " << led->deny << std::endl;
		}

		std::cout << std::endl;
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
	Logger::log(INFO, "[ServerManager] Running " + std::to_string(_servers.size()) + " servers...");
	while (!g_terminate)
	{
		signal(SIGINT, handleSigint);

		checkTimeouts();
		int ready = poll(_pollFDs.data(), _pollFDs.size(), -1);
		if (ready < 0)
		{
			if (g_terminate)
			{
				return;
			}
			perror("poll error");
			Logger::log(ERROR, "[ServerManager] Poll error");
			closeFDs();
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
			if (_pollFDs[i].revents & POLLOUT)
			{
				int clientFD = _pollFDs[i].fd;
				// Check if we have a response to this client to send
				if (_clientResponses.find(clientFD) != _clientResponses.end())
				{
					std::string &response = _clientResponses[clientFD];
					size_t responseBufferSize = WriteChunkSize;
					Logger::log(INFO, "[ServerManager] Sending response to client FD " + std::to_string(clientFD) + " ResMsg: \n" + response);

					ssize_t sBytes = send(clientFD, response.c_str(), std::min(responseBufferSize, response.size()), 0);
					if (sBytes > 0)
					{
						response.erase(0, sBytes);
					}
					else if (response.empty())
					{
						disablePollout(clientFD);
						_clientResponses.erase(clientFD);
					}
					else
					{
						perror("Failed to send response to client");
						Logger::log(ERROR, "[ServerManager] Failed to send response to client");
						_clientResponses.erase(clientFD);
						removeClient(clientFD);
					}
				}
			}
			if (_pollFDs[i].revents & POLLHUP && !(_pollFDs[i].revents & POLLIN))
			{
				Logger::log(INFO, "[ServerManager] Detected POLLHUP on FD " + std::to_string(_pollFDs[i].fd));
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
		Logger::log(ERROR, "[ServerManager] Failed to accept new client(connection)");
		throw std::runtime_error("Failed to accept new client(connection)");
	}
	Logger::log(INFO, "[ServerManager] New client connected on FD " + std::to_string(newClientFD));

	if (Server::setNonBlocking(newClientFD) == -1)
	{
		perror("[ServerManager] Failed to set client socket to non-blocking");
		Logger::log(ERROR, "[ServerManager] Failed to set client socket to non-blocking");
		close(newClientFD);
		return;
	}
	// Add new client socket to poll list, and map client FD to server
	pollfd pfd;
	pfd.fd = newClientFD;
	// Set the events to POLLIN for reading only, we will set POLLOUT when we have a response to send
	pfd.events = POLLIN;
	_pollFDs.push_back(pfd);
	_clientToServer[newClientFD] = getServerByFileDescriptor(serverFD);
	_clientActivity[newClientFD] = time(NULL); // save client activity time
}

void ServerManager::handleClientRequest(int clientFD)
{
	char buffer[ReadChunkSize];
	int rBytes = read(clientFD, buffer, sizeof(buffer) - 1);

	if (rBytes < 0)
	{
		perror("Failed to read from client");
		Logger::log(ERROR, "[ServerManager] Failed to read from client");
		removeClient(clientFD);
		return;
	}
	else if (rBytes == 0)
	{
		removeClient(clientFD);
		return;
	}
	buffer[rBytes] = '\0';
	Logger::log(INFO, "[ServerManager] Received from client FD " + std::to_string(clientFD) + '\n' + buffer);
	_clientActivity[clientFD] = time(NULL);
	_clientBuffers[clientFD].append(buffer, rBytes);
	memset(buffer, 0, sizeof(buffer));

	// Check if the request is complete
	if (isRequestComplete(_clientBuffers[clientFD]))
	{
		Logger::log(INFO, "[ServerManager] Request complete for client FD " + std::to_string(clientFD));
		std::vector<Token> tokens;
		try
		{
			tokens = HttpParser::parseRequest(_clientBuffers[clientFD]);
		}
		catch (const char *exception)
		{
			std::cerr << "Error: " << exception << std::endl;
		}
		catch (...)
		{
			std::cerr << "Unknown error" << std::endl;
		}

		_clientBuffers[clientFD].clear();
		Request request = Request(tokens);
		try
		{
			int serverFD = _clientToServer[clientFD]->getSocketFD();
			Response response(request, *getServerByFileDescriptor(serverFD));
			response.handleRequest();

			// store the response in the map
			_clientResponses[clientFD] = response.getMsg();
			Logger::log(INFO, "[ServerManager] Response for client FD " + std::to_string(clientFD) + " ResMsg: \n" + response.getMsg());

			// Set the events to POLLOUT for sending response
			enablePollout(clientFD);
		}
		catch (const std::exception &e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	else
	{
		Logger::log(INFO, "[ServerManager] Request not complete for client FD " + std::to_string(clientFD));
		return;
	}
}

void ServerManager::enablePollout(int clientFD)
{
	for (auto &pfd : _pollFDs)
	{
		if (pfd.fd == clientFD)
		{
			pfd.events |= POLLOUT;
			return;
		}
	}
}

void ServerManager::disablePollout(int clientFD)
{
	for (auto &pfd : _pollFDs)
	{
		if (pfd.fd == clientFD)
		{
			pfd.events &= ~POLLOUT;
			Logger::log(INFO, "[ServerManager] Disabled POLLOUT for client FD " + std::to_string(clientFD));
			return;
		}
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
	Logger::log(INFO, "[ServerManager] Client FD " + std::to_string(clientFD) + " disconnected.");
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
			Logger::log(INFO, "[ServerManager] Client FD " + std::to_string(clientFD) + " timed out.");
			removeClient(clientFD);
		}
	}
}

/**
 * @brief Check if the request is complete
 * @param buffer The HTTP request buffer
 * GET, HEAD does not have a body,
 * for the rest check if the Content-Length header is present and if the body is complete
 */

bool ServerManager::isRequestComplete(const std::string &buffer)
{

	size_t headerEnd = buffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
	{
		return false;
	}
	if (buffer.find("GET") == 0 || buffer.find("HEAD") == 0)
	{
		return true;
	}

	size_t pos = buffer.find("Content-Length:");
	size_t start = buffer.find(":", pos) + 1;
	size_t end = buffer.find("\r\n", start);
	if (end == std::string::npos)
	{
		return false;
	}

	std::string valueStr = buffer.substr(start, end - start);
	int contentLength = std::stoi(valueStr);

	size_t totalSize = headerEnd + 4 + contentLength;
	if (buffer.size() >= totalSize)
	{
		return true;
	}
	return false;
}

void ServerManager::handleSigint(int sig)
{
	g_terminate = sig;
	std::cout << "\nBye!" << std::endl;
}

void ServerManager::closeFDs()
{
	Logger::log(INFO, "[ServerManager] Caught SIGINT. Cleaning up...");

	for (std::map<int, Server *>::iterator it = _clientToServer.begin(); it != _clientToServer.end(); ++it)
	{
		int clientFD = it->first;
		close(clientFD);
		Logger::log(INFO, "[ServerManager] Closed client FD " + std::to_string(clientFD));
	}

	for (std::vector<std::unique_ptr<Server>>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		int serverFD = (*it)->getSocketFD();
		close(serverFD);
		Logger::log(INFO, "[ServerManager] Closed server FD " + std::to_string(serverFD));
	}

	Logger::log(INFO, "[ServerManager] Shutdown complete.");
}
