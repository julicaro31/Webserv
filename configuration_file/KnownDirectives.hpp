#ifndef __KNOWN_DIRECTIVES_H__
#define __KNOWN_DIRECTIVES_H__

#include <map>
#include "DirectiveInfo.hpp"

std::map<std::string, DirectiveInfo> knownDirectives =
{
	{"user", {{Context::GLOBAL}, "string"}},
	{"worker_processes", {{Context::GLOBAL}, "number"}},
	{"worker_connections", {{Context::EVENTS}, "number"}},
	{"listen", {{Context::SERVER}, "number"}},
	{"server_name", {{Context::SERVER}, "string"}},
	{"root", {{Context::SERVER, Context::LOCATION}, "path"}},
	{"index", {{Context::LOCATION}, "string"}},
};

#endif