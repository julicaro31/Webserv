#pragma once
#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include <vector>
#include <poll.h>
#include <unistd.h>
#include <map>
#include <memory>
#include <ServerConfig.hpp>
#include "ParsingHelper.hpp"
#include <HttpParser.hpp>

#define MAX_CONNECTION 10 // is exceeded, new connections wait in a queue (until accept() is called).
#define CLIENT_TIMEOUT 15 // disconnect clients that have been inactive, in seconds
#define MAX_BUFFER_SIZE 4096

class ServerManager
{
private:
	std::vector<std::unique_ptr<Server>> _servers;
	std::vector<pollfd> _pollFDs;
	std::map<int, Server *> _clientToServer;
	std::map<int, time_t> _clientActivity;

public:
	ServerManager();
	~ServerManager();
	// std::unique_ptr does not support copying so we cant use copy constructor and assignment operator
	ServerManager(const ServerManager &) = delete;
	ServerManager &operator=(const ServerManager &other) = delete;

	void start();
	void stop();
	const std::vector<std::unique_ptr<Server>> &getServers() const;

	Server *getServerByFileDescriptor(int fd) const;
	std::vector<pollfd> &getPollFDs();

	void printServers() const;
	void printLocations(std::vector<Location> locations) const;

	void addServer(const ServerConfig &config);
	void runPoll();
	void acceptNewClient(int serverFD);
	void handleClientRequest(int clientFD);
	void removeClient(int clientFD);
	void checkTimeouts();
	void sendErrorPage(int clientFD, int errorCode);
};

#endif