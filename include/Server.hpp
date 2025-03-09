#pragma once

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Method.hpp"
#include <algorithm>
#include <string>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <fcntl.h>
#include "Logger.hpp"
#include "Location.hpp"

#define MAX_CONNECTION 10
// If MAX_CONNECTION	 is exceeded, new connections wait in a queue (until accept() is called).

class Server
{
private:
	int _port;
	int _socketFD;
	bool _autoIndex;
	bool _defaultServer;
	std::string _host;
	std::string _root;
	std::vector<std::string> _index;
	size_t _maxBodySize;
	std::string _serverName;
	std::pair<int, std::string> _redirection;
	std::map<int, std::string> _errorPages;
	std::vector<Location> _locations;

public:
	// still not sure if this would be the right way to do it? or use another the setters to set the values
	Server();
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();

	const std::string &getHost() const;
	bool operator==(const Server &other) const;
	bool setupSocket();

	// Setters
	void setHost(const std::string &host);
	void setPort(int port);
	void setMaxBodySize(size_t maxBodySize);
	void setRoot(const std::string &root);
	void setIndex(const std::vector<std::string> &index);
	void setAutoIndex(bool autoindex);
	void setErrorPages(const std::map<int, std::string> &errorPages);
	void setDefaultServer(bool defaultServer);
	void setLocations(std::vector<Location> locations);

	// Getters
	int getPort() const;
	const std::string &getRoot() const;
	const std::vector<std::string> &getIndex() const;
	bool isAutoIndexEnabled() const;
	const std::map<int, std::string> &getErrorPages() const;
	size_t getMaxBodySize() const;
	std::string getErrorPage(int statusCode) const;
	int getSocketFD() const;
	static int setNonBlocking(int fd);
};

#endif