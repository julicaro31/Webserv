#pragma once

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <iostream>
#include <map>

class Server
{
private:
	int _port;
	int _socketFD;
	bool _autoIndex;
	std::string _host;
	std::string _root;
	std::string _index;
	size_t _maxBodySize;
	std::map<int, std::string> _errorPages;

public:
	// still not sure if this would be the right way to do it? or use another the setters to set the values
	Server() : _socketFD(-1) {}
	Server(const Server &other);
	Server &operator=(const Server &other);
	~Server();

	const std::string &getHost() const;

	// Setters
	void setHost(const std::string &host);
	void setPort(int port);
	void setMaxBodySize(size_t maxBodySize);
	void setRoot(const std::string &root);
	void setIndex(const std::string &index);
	void setAutoIndex(bool autoindex);
	void setErrorPages(const std::map<int, std::string> &errorPages);

	// Getters
	int getPort() const;
	const std::string &getRoot() const;
	const std::string &getIndex() const;
	bool isAutoIndexEnabled() const;
	size_t getMaxBodySize() const;
	std::string getErrorPage(int statusCode) const;
};

#endif