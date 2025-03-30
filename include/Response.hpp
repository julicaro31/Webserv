#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "Server.hpp"
#include "ParsingHelper.hpp"

class Response
{
    public:

        void setConfiguration(const std::string uri, Server& server);
       
        bool isCGI(const std::string& uri) const;

    private:
        bool _autoIndex;
        size_t _maxBodySize;
        std::string _root;
        std::pair<int, std::string> _redirection;
        std::map<int, std::string> _errorPages;
        std::vector<std::string> _index;
        std::map<std::string, std::string> _cgiExtensionMap;
        std::vector<LimitExceptDirective> _limitExcepts;
};

#endif