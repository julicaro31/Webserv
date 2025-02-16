#pragma once

#include <string>
#include <map>

struct ServerConfig {
    bool autoIndex;
    bool defaultServer;
    size_t maxBodySize;
    int port;
    std::string host;
    std::string index;
    std::string root;
    std::map<int, std::string> errorPages;
};;
