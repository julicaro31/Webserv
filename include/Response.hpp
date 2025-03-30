#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "Server.hpp"
#include "ParsingHelper.hpp"

class Response
{
public:
	// handleRequest();

private:
	void setConfiguration(const std::string uri, Server &server);

	void handleGetRequest(const std::string &uri);
	void handlePostRequest(const std::string &uri);
	void handleDeleteRequest(const std::string &uri);

	bool isCGI(const std::string &uri) const;
	bool isAllowed(Method method) const;
	bool isFile(const std::string &uri) const;

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