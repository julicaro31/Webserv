#include "Directives.hpp"

std::map<std::string, std::vector<Context>> Directives =
{
	{"listen", {Context::SERVER}},
	{"server_name", {Context::SERVER}},
	{"error_page", {Context::HTTP, Context::SERVER, Context::LOCATION}},
	{"client_max_body_size", {Context::HTTP, Context::SERVER, Context::LOCATION}},
	{"limit_except", {Context::LOCATION}},
	{"return", {Context::SERVER, Context::LOCATION}},
	{"root", {Context::HTTP, Context::SERVER, Context::LOCATION}},
	{"autoindex", {Context::HTTP, Context::SERVER, Context::LOCATION}},
	{"index", {Context::HTTP, Context::SERVER, Context::LOCATION}}
};