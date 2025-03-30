#include "Response.hpp"

void Response::setConfiguration(const std::string uri, Server& server)
{
    const std::vector<Location>& locations = server.getLocations();
    const Location* bestMatch = nullptr;
    size_t bestMatchLength = 0;

    for (std::vector<Location>::const_iterator location = locations.begin(); location != locations.end(); location++)
    {
        if (uri.find(location->uri) == 0)
        {
            if (location->modifier == "=" && location->uri == uri)
            {
                bestMatch = &(*location);
                break;
            }
            if (location->modifier == "" && location->uri.length() > bestMatchLength)
            {
                bestMatch = &(*location);
                bestMatchLength = location->uri.length();
            }
        }
    }

    if (bestMatch == nullptr)
    {
        _autoIndex = server.isAutoIndexEnabled();
        _maxBodySize = server.getMaxBodySize();
        _root = server.getRoot();
        _redirection = server.getRedirection();
        _errorPages = server.getErrorPages();
        _index = server.getIndex();
        _cgiExtensionMap = std::map<std::string, std::string>();
        _limitExcepts = {};
    }
    else
    {
        _autoIndex = bestMatch->autoIndex;
        _maxBodySize = bestMatch->maxBodySize;
        _root = bestMatch->root;
        _redirection = bestMatch->redirection;
        _errorPages = bestMatch->errorPages;
        _index = bestMatch->index;
        _cgiExtensionMap = bestMatch->cgiExtensionMap;
        _limitExcepts = bestMatch->limitExcepts;
    }
}

bool Response::isCGI(const std::string& uri) const
{
    const size_t extensionPos = uri.find_last_of('.');
    if (extensionPos == std::string::npos)
    {
        return false;
    }
    std::string extension = uri.substr(extensionPos);
    if (_cgiExtensionMap.find(extension) != _cgiExtensionMap.end())
    {
        return true;
    }
    return false;
}

