#ifndef __LOCATION_H__
#define __LOCATION_H__

#include "ConfigBlock.hpp"
#include "LimitExceptDirective.hpp"

class Location : public ConfigBlock
{
public:
	Location(std::string uri, std::string modifier, std::vector<LimitExceptDirective> limitExcepts, std::map<std::string, std::vector<std::string>> directives);

	const std::string getUri() const;
	const std::string getModifier() const;
	const std::vector<LimitExceptDirective> getLimitExcepts() const;

private:
	std::string _uri;
	std::string _modifier;
	std::vector<LimitExceptDirective> _limitExcepts;
};

#endif