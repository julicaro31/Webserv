#include "Server.hpp"

Server::Server() : _port(0), _socketFD(-1), _autoIndex(false), _maxBodySize(0)
{
  (void)_socketFD;
}

Server::Server(const Server &other) { *this = other; }

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
    _errorPages =
        other._errorPages; // Assuming there's no getter for _errorPages
  }
  return *this;
}

bool Server::operator==(const Server &other) const
{
  return this->getHost() == other.getHost() &&
         this->getPort() == other.getPort();
}

// Destructor
Server::~Server()
{
  // If you decide that Server should manage the socket, you could close it
  // here. For example: if (_socketFD != -1)
  // {
  //     close(_socketFD);
  // }
}

// Getters
const std::string &Server::getHost() const { return _host; }

int Server::getPort() const { return _port; }

bool Server::isAutoIndexEnabled() const { return _autoIndex; }

bool Server::isDefaultServer() const { return _defaultServer; }

size_t Server::getMaxBodySize() const { return _maxBodySize; }

const std::string &Server::getRoot() const { return _root; }

const std::string &Server::getServerName() const { return _serverName; }

const std::pair<int, std::string> Server::getRedirection() const
{
  return _redirection;
}

const std::map<int, std::string> &Server::getErrorPages() const
{
  return _errorPages;
}

std::string Server::getErrorPage(int statusCode) const
{
  std::map<int, std::string>::const_iterator it = _errorPages.find(statusCode);
  if (it != _errorPages.end())
    return it->second;
  return "";
}

const std::vector<std::string> &Server::getIndex() const { return _index; }

std::vector<Location> Server::getLocations() const { return _locations; }

int Server::getSocketFD() const { return _socketFD; }

// Setters
void Server::setHost(const std::string &host) { _host = host; }

void Server::setPort(int port) { _port = port; }

void Server::setAutoIndex(bool enableAutoIndex)
{
  _autoIndex = enableAutoIndex;
}

void Server::setDefaultServer(bool defaultServer)
{
  _defaultServer = defaultServer;
}

void Server::setMaxBodySize(size_t maxBodySize) { _maxBodySize = maxBodySize; }

void Server::setRoot(const std::string &root) { _root = root; }

void Server::setServerName(std::string serverName) { _serverName = serverName; }

void Server::setRedirection(std::pair<int, std::string> redirection)
{
  _redirection = redirection;
}

void Server::setErrorPages(const std::map<int, std::string> &errorPages)
{
  _errorPages = errorPages;
}

void Server::setIndex(const std::vector<std::string> &index) { _index = index; }
void Server::setLocations(std::vector<Location> locations)
{
  _locations = locations;
}

/**
 * @brief This function should create a socket and bind it to the server's host
 * and port.
 *
 * @return true if the socket was created successfully
 * @return false if failed to create the socket
 */
bool Server::setupSocket()
{

  _socketFD = socket(AF_INET, SOCK_STREAM, 0);
  if (_socketFD < 0)
  {
    Logger::log(ERROR, "[Server] Socket creation failed");
    return false;
  }

  int opt = 1;
  if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) ==
      -1)
  {
    Logger::log(ERROR, "[Server] Failed to setsocketopt");
    close(_socketFD);
    return false;
  }

  if (setNonBlocking(_socketFD) == -1)
  {
    Logger::log(ERROR, "[Server] Failed to set socket to non-blocking");
    close(_socketFD);
    return false;
  }

  struct addrinfo hints{}, *res;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  std::string portStr = std::to_string(_port);
  int status = getaddrinfo(_host.c_str(), portStr.c_str(), &hints, &res);
  if (status != 0)
  {
    Logger::log(ERROR, "getaddrinfo error");
    return false;
  }

  struct sockaddr_in _serverAddr;

  memset(&_serverAddr, 0, sizeof(_serverAddr));
  _serverAddr = *(struct sockaddr_in *)res->ai_addr;

  if (bind(_socketFD, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) <
      0)
  {
    Logger::log(ERROR, "[Server Bind failed]");
    close(_socketFD);
    return false;
  }
  // The exact value of SOMAXCONN is determined by the system and may vary
  // between Mac and Linux.
  if (listen(_socketFD, SOMAXCONN) < 0)
  {
    Logger::log(ERROR, "[Server] Listen failed");
    close(_socketFD);
    return false;
  }

  Logger::log(INFO,
              "[Server] Listening on " + _host + ":" + std::to_string(_port));
  return true;
}

int Server::setNonBlocking(int fd)
{
  int currFlag = fcntl(fd, F_GETFL);
  if (currFlag == -1)
    return -1;
  return (fcntl(fd, F_SETFL, currFlag | O_NONBLOCK));
}
