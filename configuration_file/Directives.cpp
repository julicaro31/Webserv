#include "Directives.hpp"

/// @brief Directives required to handle.
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

/// @brief Directives that can only appear once per block not to cause ambiguities.
std::vector<std::string> SingleUseDirectives = {"server_name", "client_max_body_size", "root", "autoindex"};

/// @brief Directives that can only have one value per block. Server_name can have multiple in the same line but the last one will be taken into account.
std::vector<std::string> SingleValueDirectives = {"client_max_body_size", "root", "autoindex"};