#include "Directives.hpp"

/// @brief Directives required to handle.
const std::map<std::string, std::vector<Context>> Directives =
	{
		{"listen", {Context::SERVER}},
		{"server_name", {Context::SERVER}},
		{"error_page", {Context::HTTP, Context::SERVER, Context::LOCATION}},
		{"client_max_body_size", {Context::HTTP, Context::SERVER, Context::LOCATION}},
		{"allow", {Context::LIMIT_EXCEPT}},
		{"deny", {Context::LIMIT_EXCEPT}},
		{"return", {Context::SERVER, Context::LOCATION}},
		{"root", {Context::HTTP, Context::SERVER, Context::LOCATION}},
		{"autoindex", {Context::HTTP, Context::SERVER, Context::LOCATION}},
		{"index", {Context::HTTP, Context::SERVER, Context::LOCATION}},
		{"cgi_pass", {Context::LOCATION}}};

/// @brief Directives that can only appear once per block not to cause ambiguities.
const std::vector<std::string> SingleUseDirectives = {"listen", "server_name", "client_max_body_size", "root", "autoindex", "return", "cgi_pass"};

/// @brief Directives that can only have one value per block.
const std::vector<std::string> SingleValueDirectives = {"listen", "client_max_body_size", "root", "autoindex", "cgi_pass"};