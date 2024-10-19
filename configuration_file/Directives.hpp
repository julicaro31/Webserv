#ifndef __DIRECTIVES_H__
#define __DIRECTIVES_H__

#include <map>
#include <vector>
#include <iostream>
#include "Context.hpp"

std::map<std::string, std::vector<Context>> Directives =
{
	{"listen", {Context::server}},
	{"server_name", {Context::server}},
	{"error_page", {Context::http, Context::server, Context::location}},
	{"client_max_body_size", {Context::http, Context::server, Context::location}},
	{"limit_except", {Context::location}},
	{"return", {Context::server, Context::location}},
	{"root", {Context::http, Context::server, Context::location}},
	{"autoindex", {Context::http, Context::server, Context::location}},
	{"index", {Context::http, Context::server, Context::location}}
};

#endif