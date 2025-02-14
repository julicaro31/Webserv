#pragma once
#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <map>

class ServerManager
{
private:
	std::vector<Server> _servers;
	std::vector<pollfd> _pollFDs;
	std::map<int, Server *> _serverFDs;
	std::map<int, time_t> _clientActivity;

public:
	ServerManager();
	ServerManager(const ServerManager &other);
	ServerManager &operator=(const ServerManager &other);
	~ServerManager();

	void addServer(const Server &server);
	void removeServer(const Server &server);
	void start();
	void stop();
	const std::vector<Server> &getAllServers() const;
	Server *getServerByFileDescriptor(int fd) const;
	std::vector<pollfd> &getPollFDs();
};

#endif