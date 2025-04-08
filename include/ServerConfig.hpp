#pragma once

#include "Location.hpp"
#include <string>
#include <map>

struct ServerConfig
{
    int port;
    bool autoIndex;
    bool defaultServer;
    size_t maxBodySize;
    std::string host;
    std::string root;
    std::string serverName;
    std::pair<int, std::string> redirection;
    std::map<int, std::string> errorPages;
    std::vector<std::string> index;
    std::vector<Location> locations;
};
