#pragma once

#include "Location.hpp"
#include <string>
#include <map>

struct ServerConfig
{
    bool autoIndex;
    bool defaultServer;
    size_t maxBodySize;
    int port;
    std::string host;
    std::vector<std::string> index;
    std::string root;
    std::string serverName;
    std::pair<int, std::string> redirection;
    std::map<int, std::string> errorPages;
    std::vector<Location> locations;
};
