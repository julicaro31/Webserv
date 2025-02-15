#pragma once

#include <string>
#include <map>

struct ServerConfig {
    std::string host;
    int port;
    std::string root;
    std::string index;
    bool autoIndex;
    size_t maxBodySize;
    std::map<int, std::string> errorPages;
};
