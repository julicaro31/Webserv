#include "Location.hpp"

Location::Location(std::string uri, std::string modifier, std::vector<LimitExceptDirective> limitExcepts, std::map<std::string, std::vector<std::string>> directives) : ConfigBlock(directives), _uri{uri}, _modifier{modifier}, _limitExcepts(limitExcepts) {}

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