#ifndef __LOCATION_H__
#define __LOCATION_H__

#include "LimitExceptDirective.hpp"

struct Location
{
	std::string uri;
	std::string modifier;

	bool autoIndex;
	std::string root;
	size_t maxBodySize;
	std::vector<std::string> index;
	std::pair<int, std::string> redirection;
	std::map<int, std::string> errorPages;
	std::map<std::string, std::string> cgiExtensionMap;
	std::vector<LimitExceptDirective> limitExcepts;
};

#endif