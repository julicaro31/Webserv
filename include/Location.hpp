#ifndef __LOCATION_H__
#define __LOCATION_H__

#include "ConfigBlock.hpp"
#include "LimitExceptDirective.hpp"

struct ServerConfig;

class Location : public ConfigBlock
{
public:
	Location(std::string uri, std::string modifier, std::vector<LimitExceptDirective> limitExcepts, ServerConfig directives);

	const std::string getUri() const;
	const std::string getModifier() const;
	const std::vector<LimitExceptDirective> getLimitExcepts() const;

private:
	std::string _uri;
	std::string _modifier;
	std::vector<LimitExceptDirective> _limitExcepts;

	bool _autoIndex;
	std::string _root;
	size_t _maxBodySize;
	std::vector<std::string> _index;
	std::map<int, std::string> _errorPages;
	std::pair<int, std::string> _redirection;
	std::map<std::string, std::string> _cgiExtensionMap;
};

#endif