#include "Location.hpp"
#include "ServerConfig.hpp"

Location::Location(std::string uri, std::string modifier, std::vector<LimitExceptDirective> limitExcepts, ServerConfig directives) : _uri{uri}, _modifier{modifier}, _limitExcepts(limitExcepts)
{
	_autoIndex = directives.autoIndex;
	_root = directives.root;
	_maxBodySize = directives.maxBodySize;
	_index = directives.index;
	_errorPages = directives.errorPages;
	_redirection = directives.redirection;
	_cgiExtensionMap = directives.cgiExtensionMap;
}

const std::string Location::getUri() const
{
	return _uri;
}

const std::string Location::getModifier() const
{
	return _modifier;
}

const std::vector<LimitExceptDirective> Location::getLimitExcepts() const
{
	return _limitExcepts;
}