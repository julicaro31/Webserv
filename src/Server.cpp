#include "Server.hpp"

Server::Server() : _port(0), _autoIndex(false), _maxBodySize(0)
{
}

Server::Server(const Server &other)
{
	*this = other;
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_host = other.getHost();
		_port = other.getPort();
		_root = other.getRoot();
		_index = other.getIndex();
		_autoIndex = other.isAutoIndexEnabled();
		_maxBodySize = other.getMaxBodySize();
		_errorPages = other._errorPages; // Assuming there's no getter for _errorPages
	}
	return *this;
}

// Destructor
Server::~Server()
{
	// If you decide that Server should manage the socket, you could close it here.
	// For example:
	// if (_socketFD != -1)
	// {
	//     close(_socketFD);
	// }
}

// Getters
const std::string &Server::getHost() const
{
	return _host;
}

int Server::getPort() const
{
	return _port;
}

const std::string &Server::getRoot() const
{
	return _root;
}

const std::string &Server::getIndex() const
{
	return _index;
}

bool Server::isAutoIndexEnabled() const
{
	return _autoIndex;
}

size_t Server::getMaxBodySize() const
{
	return _maxBodySize;
}

std::string Server::getErrorPage(int statusCode) const
{
	std::map<int, std::string>::const_iterator it = _errorPages.find(statusCode);
	if (it != _errorPages.end())
		return it->second;
	return "";
}

// Setters
void Server::setHost(const std::string &host)
{
	_host = host;
}

void Server::setPort(int port)
{
	_port = port;
}

void Server::setRoot(const std::string &root)
{
	_root = root;
}

void Server::setIndex(const std::string &index)
{
	_index = index;
}

void Server::setAutoIndex(bool enableAutoIndex)
{
	_autoIndex = enableAutoIndex;
}

void Server::setMaxBodySize(size_t maxBodySize)
{
	_maxBodySize = maxBodySize;
}

void Server::setErrorPages(const std::map<int, std::string> &errorPages)
{
	_errorPages = errorPages;
}
