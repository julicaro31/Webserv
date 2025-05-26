#include "ParsingHelper.hpp"
#include "ConfigBlock.hpp"

/// @brief Parses a configuration file.
/// @param configFilePath Path of the configuration file.
/// @return The parsed configuration file stored in a ConfigBlock.
/// @throw std::runtime_error if the file path is invalid or could not be opened.
ConfigBlock ParsingHelper::parseConfigFile(std::string &configFilePath)
{
	std::ifstream configFile(configFilePath);

	if (!configFile.is_open())
	{
		Logger::log(ERROR, "Could not open file.");
		throw std::runtime_error("Error: Could not open file.");
	}

	ConfigBlock rootBlock = parseBlock(configFile, toString(Context::NONE));
	configFile.close();
	return rootBlock;
}

/// @brief Parses one block of a configuration file, meaning a context block.
/// @param file Input configuration file stream, starting from the line containing the block to parse.
/// @param blockName Name of the block/context.
/// @param braceLevel How many braces have been opened.
/// @return The parsed configuration block stored in a ConfigBlock variable.
/// @throw std::runtime_error if there is an error related to syntax or invalid values.
ConfigBlock ParsingHelper::parseBlock(std::ifstream &file, std::string blockName, int braceLevel)
{
	std::string line;
	ConfigBlock block;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
		{
			continue;
		}

		if (line.back() == ';')
		{
			handleDirective(block, blockName, line);
		}
		else if (line.back() == '{')
		{
			handleSubBlock(file, block, blockName, line);
		}
		else if (line.back() == '}' && trim(line).size() == 1)
		{
			braceLevel--;
			if (braceLevel == 0)
			{
				return block;
			}
			else
			{
				Logger::log(ERROR, "Unmatched closing brace.");
				throw std::runtime_error("Error: Unmatched closing brace.");
			}
		}
		else
		{
			Logger::log(ERROR, "Incorrect syntax in file.");
			throw std::runtime_error("Error: Incorrect syntax in file.");
		}
	}
	if (braceLevel > 0)
	{
		Logger::log(ERROR, "Missing closing brace.");
		throw std::runtime_error("Error: Missing closing brace.");
	}
	return block;
}

/// @brief Parses line of configuration file containing info related to a directive and adds it to the ConfigBlock.
/// @param block ConfigBlock to add the directive.
/// @param blockName Name of the context representing the ConfigBlock.
/// @param line Contains info about a directive.
void ParsingHelper::handleDirective(ConfigBlock &block, std::string blockName, std::string line)
{
	line.pop_back();
	std::stringstream ss(line);
	std::string key, value;
	ss >> key;
	getline(ss, value);
	block.addDirective(key, trim(value), blockName.substr(0, blockName.find(" ")));
}

/// @brief Parses a sub-block starting from a specific point in the configuration file and adds it to its parent block.
/// @param file Input configuration file stream, starting from the line containing the sub-block to parse.
/// @param block Parent block to which the parsed sub-block will be added.
/// @param blockName Name of the parent block.
/// @param line Line containing the sub-block's name. Example: "http {"
void ParsingHelper::handleSubBlock(std::ifstream &file, ConfigBlock &block, std::string blockName, std::string line)
{
	std::string subBlockName = trim(line.substr(0, line.size() - 1));
	ConfigBlock subBlock = parseBlock(file, subBlockName, 1);
	block.addSubBlock(subBlockName, blockName, subBlock);
}

/// @brief Removes outer whitespaces.
/// @param str String to trim.
/// @return The trimmed string.
std::string ParsingHelper::trim(const std::string &str)
{
	std::string::const_iterator start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch)
														 { return std::isspace(ch); });

	std::string::const_iterator end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch)
													   { return std::isspace(ch); })
										  .base();

	if (start >= end)
	{
		return "";
	}

	return std::string(start, end);
}

/// @brief Converts the context to a string.
/// @param context Contex to convert.
/// @return The context as a string.
std::string ParsingHelper::toString(Context context)
{
	switch (context)
	{
	case Context::HTTP:
		return "http";
	case Context::SERVER:
		return "server";
	case Context::LOCATION:
		return "location";
	case Context::LIMIT_EXCEPT:
		return "limit_except";
	case Context::NONE:
		return "none";
	default:
		return "unknown";
	}
}

/// @brief Splits a string into a vector of substrings based on a specified delimiter.
/// @param str The input string to be split.
/// @param delimiter The character that separates the substrings in the input string.
/// @return A vector of strings, where each element is a substring from the original string, split at each occurrence of the delimiter.
std::vector<std::string> ParsingHelper::split(const std::string &str, char delimiter)
{
	std::vector<std::string> words;
	std::stringstream ss(str);
	std::string word;

	while (std::getline(ss, word, delimiter))
	{
		words.push_back(word);
	}

	return words;
}

/// @brief Converts a string to the corresponding method.
/// @param method Method as a string to convert.
/// @return The string as a 'Method'.
Method ParsingHelper::strToMethod(std::string method)
{
	if (method == "GET")
	{
		return Method::GET;
	}
	if (method == "POST")
	{
		return Method::POST;
	}
	if (method == "DELETE")
	{
		return Method::DELETE;
	}
	return Method::NONE;
}

/// @brief Converts a Method to string.
/// @param method Method to convert.
/// @return The method as a string.
std::string ParsingHelper::methodToStr(Method method)
{
	if (method == Method::GET)
	{
		return "GET";
	}
	if (method == Method::POST)
	{
		return "POST";
	}
	if (method == Method::DELETE)
	{
		return "DELETE";
	}
	return "";
}

/// @brief Parses while validating the values of a configuration file, whose path is given.
/// @param configFilePath Path of the configuration file to parse.
/// @return Vector of ServerConfig's. Each vector's element holds data from a server, parsed from the configuration file.
std::vector<ServerConfig> ParsingHelper::getServersConfig(std::string &configFilePath)
{
	ConfigBlock configFile = ParsingHelper::parseConfigFile(configFilePath);
	// configFile.print();

	std::vector<ServerConfig> serversConfig;
	ConfigBlock httpBlock = configFile.getConfigBlocksByContext(Context::HTTP)[0];
	std::vector<ConfigBlock> serverConfigBlocks = httpBlock.getConfigBlocksByContext(Context::SERVER);

	bool firstServer = true;
	for (std::vector<ConfigBlock>::iterator serverBlock = serverConfigBlocks.begin(); serverBlock != serverConfigBlocks.end(); serverBlock++)
	{
		ServerConfig serverConfig;
		serverConfig.defaultServer = firstServer;

		parseDirectives(serverBlock->getDirectives(), serverConfig);

		parseLocation(*serverBlock, serverConfig);

		serversConfig.push_back(serverConfig);

		firstServer = false;
	}
	return serversConfig;
}

/// @brief Parses and adds the location data from the given server ConfigBlock to the given ServerConfig.
/// @param serverBlock ConfigBlock to get the location blocks from.
/// @param serverConfig ServerConfig to add the locations.
/// @throw std::invalid_argument if there is a syntax error related to location.
void ParsingHelper::parseLocation(ConfigBlock &serverBlock, ServerConfig &serverConfig)
{
	std::vector<Location> locationBlocks;

	std::map<std::string, std::vector<ConfigBlock>> configBlocks = serverBlock.getSubConfigBlocks();

	for (std::map<std::string, std::vector<ConfigBlock>>::iterator it = configBlocks.begin(); it != configBlocks.end(); it++)
	{
		std::vector<std::string> contextInfo = ParsingHelper::split(it->first, ' ');

		if (contextInfo[0] == "location")
		{
			int contextInfoSize = contextInfo.size();

			if (contextInfoSize != 2 && contextInfoSize != 3)
			{
				Logger::log(ERROR, "location syntax error.");
				throw std::invalid_argument("Error: location syntax error");
			}

			std::string modifier = contextInfoSize == 3 ? contextInfo[1] : "";
			std::string uri = contextInfo.back();

			if (modifier != "=" && modifier != "")
			{
				Logger::log(ERROR, "listen modifier must be '=' or empty. Regex is not available on this project.");
				throw std::invalid_argument("Error: listen modifier must be '=' or empty. Regex is not available on this project.");
			}

			for (std::vector<ConfigBlock>::iterator locationBlock = it->second.begin(); locationBlock != it->second.end(); locationBlock++)
			{
				Location locationConfig;
				locationConfig.uri = uri;
				locationConfig.modifier = modifier;
				locationConfig.root = serverConfig.root;
				locationConfig.errorPages = serverConfig.errorPages;
				locationConfig.maxBodySize = serverConfig.maxBodySize;
				locationConfig.index = serverConfig.index;
				locationConfig.autoIndex = serverConfig.autoIndex;
				parseDirectives(locationBlock->getDirectives(), locationConfig);
				locationConfig.limitExcepts = parseLimitExcepts(*locationBlock);
				locationBlocks.push_back(locationConfig);
			}
		}
	}

	serverConfig.locations = locationBlocks;
}

/// @brief Parses and gets data related to the 'limit_except' directive.
/// @param locationBlock Location ConfigBlock to get the data from.
/// @return std::vector of 'limit_except' data stored in 'LimitExceptDirective'.
/// @throw std::invalid_argument if the limit_except contains more than one method or none.
std::vector<LimitExceptDirective> ParsingHelper::parseLimitExcepts(ConfigBlock &locationBlock)
{
	std::vector<LimitExceptDirective> limitExcepts;

	std::map<std::string, std::vector<ConfigBlock>> subConfigsBlocks = locationBlock.getSubConfigBlocks();

	for (std::map<std::string, std::vector<ConfigBlock>>::iterator it = subConfigsBlocks.begin(); it != subConfigsBlocks.end(); it++)
	{
		std::vector<std::string> contextInfo = ParsingHelper::split(it->first, ' ');

		if (contextInfo[0] == "limit_except")
		{
			if (contextInfo.size() < 2)
			{
				Logger::log(ERROR, "limit_except must contain at least one method.");
				throw std::invalid_argument("Error: limit_except must contain at least one method.");
			}

			for (std::vector<ConfigBlock>::iterator limitExceptBlock = it->second.begin(); limitExceptBlock != it->second.end(); limitExceptBlock++)
			{
				LimitExceptDirective limitExcept;
				limitExcept.methods = parseMethods(contextInfo);

				std::map<std::string, std::vector<std::string>>::iterator allowFinder = limitExceptBlock->getDirectives().find("allow");
				limitExcept.allow = allowFinder != limitExceptBlock->getDirectives().end() ? allowFinder->second[0] : "";

				std::map<std::string, std::vector<std::string>>::iterator denyFinder = limitExceptBlock->getDirectives().find("deny");
				limitExcept.deny = denyFinder != limitExceptBlock->getDirectives().end() ? denyFinder->second[0] : " ";

				limitExcepts.push_back(limitExcept);
			}
		}
	}

	return limitExcepts;
}

/// @brief Parses the methods from the limit_except directive.
/// @param methods Vector of strings containing the methods.
/// @return A vector of Method.
/// @throw std::invalid_argument if any of the methods is not valid.
std::vector<Method> ParsingHelper::parseMethods(std::vector<std::string> &methods)
{
	std::vector<Method> parsedMethods;

	for (std::vector<std::string>::iterator it = methods.begin() + 1; it != methods.end(); it++)
	{
		Method method = strToMethod(*it);
		if (method == Method::NONE)
		{
			Logger::log(ERROR, "Invalid method in limit_except directive.");
			throw std::invalid_argument("Error: Invalid method in limit_except directive.");
		}
		parsedMethods.push_back(method);
	}

	return parsedMethods;
}

/// @brief Parses, validates and adds the directives to the given ServerConfig.
/// @param directives Directives to be added.
/// @param serverConfig ServerConfig to add the directives.
/// @throw std::runtime_error if not all mandatory directives have been parsed.
void ParsingHelper::parseDirectives(std::map<std::string, std::vector<std::string>> &directives, ServerConfig &serverConfig)
{
	setDefaultValues(serverConfig);
	std::vector<std::string> directivesSet;

	for (std::map<std::string, std::vector<std::string>>::iterator directive = directives.begin(); directive != directives.end(); directive++)
	{
		std::string directiveName = directive->first;
		if (directiveName == "autoindex")
		{
			serverConfig.autoIndex = parseAutoIndex(directive->second[0]);
		}
		else if (directiveName == "listen")
		{
			std::pair<std::string, int> hostAndPort = parseHostAndPort(directive->second[0]);
			serverConfig.host = hostAndPort.first;
			serverConfig.port = hostAndPort.second;
		}
		else if (directiveName == "root")
		{
			serverConfig.root = directive->second[0];
		}
		else if (directiveName == "index")
		{
			serverConfig.index = directive->second;
		}
		else if (directiveName == "client_max_body_size")
		{
			serverConfig.maxBodySize = parseMaxBodySize(directive->second[0]);
		}
		else if (directiveName == "server_name")
		{
			serverConfig.serverName = directive->second[0];
		}
		else if (directiveName == "return")
		{
			serverConfig.redirection = parseReturn(directive->second);
		}
		else if (directiveName == "error_page")
		{
			serverConfig.errorPages = parseErrorPages(directive->second);
		}

		directivesSet.push_back(directiveName);
	}

	if ((std::find(directivesSet.begin(), directivesSet.end(), "listen") == directivesSet.end() || std::find(directivesSet.begin(), directivesSet.end(), "root") == directivesSet.end()))
	{
		Logger::log(ERROR, "No all mandatory directives have been set.");
		throw std::runtime_error("Error: No all mandatory directives have been set.");
	}

	if (std::find(directivesSet.begin(), directivesSet.end(), "server_name") == directivesSet.end())
	{
		serverConfig.serverName = serverConfig.host;
	}
}

/// @brief Parses, validates and adds the directives to the given Location.
/// @param directives Directives to be added.
/// @param locationConfig Location to add the directives.
void ParsingHelper::parseDirectives(std::map<std::string, std::vector<std::string>> &directives, Location &locationConfig)
{
	std::vector<std::string> directivesSet;

	for (std::map<std::string, std::vector<std::string>>::iterator directive = directives.begin(); directive != directives.end(); directive++)
	{
		std::string directiveName = directive->first;
		if (directiveName == "autoindex")
		{
			locationConfig.autoIndex = parseAutoIndex(directive->second[0]);
		}
		else if (directiveName == "root")
		{
			locationConfig.root = directive->second[0];
		}
		else if (directiveName == "index")
		{
			locationConfig.index = directive->second;
		}
		else if (directiveName == "client_max_body_size")
		{
			locationConfig.maxBodySize = parseMaxBodySize(directive->second[0]);
		}
		else if (directiveName == "return")
		{
			locationConfig.redirection = parseReturn(directive->second);
		}
		else if (directiveName == "error_page")
		{
			locationConfig.errorPages = parseErrorPages(directive->second);
		}
		else if (directiveName == "cgi_pass")
		{
			locationConfig.cgiPass = directive->second[0];
		}

		directivesSet.push_back(directiveName);
	}
}

/// @brief Sets default values for a configuration.
/// @param config The config to set the values for.
template <typename T>
void ParsingHelper::setDefaultValues(T &config)
{
	config.autoIndex = false;
	config.index = {};
	config.maxBodySize = 1000000;
	config.redirection = {0, ""};
	config.errorPages = {{404, "/404.html"}, {403, "/403.html"}, {500, "/50x.html"}, {502, "/50x.html"}, {503, "/50x.html"}, {504, "/50x.html"}};
}

/// @brief Parses the string value related to the directive 'autoindex'.
/// @param value Value of 'autoindex' as a string.
/// @return True if the value is 'on', false if 'off'.
/// @throw std::invalid_argument if the value is neither "on" nor "off".
bool ParsingHelper::parseAutoIndex(std::string &value)
{
	if (value == "on")
	{
		return true;
	}
	if (value == "off")
	{
		return false;
	}
	Logger::log(ERROR, "Autoindex value must be either 'on' or 'off'.");
	throw std::invalid_argument("Error: Autoindex value must be either 'on' or 'off'.");
}

/// @brief Parses the string value related to the directive 'client_max_body_size'.
/// @param value Value of 'client_max_body_size' as a string.
/// @return Value converted to size_t.
/// @throw std::invalid_argument if the string is not a positive number.
size_t ParsingHelper::parseMaxBodySize(std::string &value)
{
    long long int maxBodySize;
	try
	{
        if (value.size() > 8)
        {
            Logger::log(ERROR, "client_max_body_size: size too big");
            throw std::invalid_argument("Error: client_max_body_size size too big");
        }
		maxBodySize = std::stoi(value);
		if (maxBodySize <= 0)
		{
            Logger::log(ERROR, "client_max_body_size should be a positive integer.");
            throw std::invalid_argument("Error: client_max_body_size should be a positive integer.");
		}
	}
	catch (const std::exception &e)
	{
		Logger::log(ERROR, std::string("client_max_body_size: ") + e.what());
        throw ;
	}
    return maxBodySize;
}

/// @brief Parses the information realated to the directive 'listen'.
/// @param info String that holds data of the host and port.
/// @return A std::pair containing the host and port. If the host is not provided in info, it returns a default value.
/// @throw std::invalid_argument if the port is invalid.
std::pair<std::string, int> ParsingHelper::parseHostAndPort(std::string &info)
{
	std::vector<std::string> values = split(info, ':');
	int valuesSize = values.size();

	if (valuesSize > 2)
	{
		Logger::log(ERROR, "Invalid host and port.");
		throw std::invalid_argument("Error: Invalid host and port.");
	}

	try
	{
		std::string portStr = valuesSize == 2 ? values[1] : values[0];
		int port = std::stoi(portStr);

		if (port < 1024 || port > 65536)
		{
			Logger::log(ERROR, "Port number should be between 1024 and 65535.");
			throw std::invalid_argument("Error: Port number should be between 1024 and 65535.");
		}

		std::string host = valuesSize == 2 ? values[0] : "0.0.0.0";

		return std::make_pair(host, port);
	}
	catch (const std::exception &e)
	{
		Logger::log(ERROR, "Invalid port.");
		throw std::invalid_argument("Error: Invalid port.");
	}
}

/// @brief Parses string values related to the directive 'return'.
/// @param values Holds data related to an http redirection.
/// @return A std::pair containing a status code value and an url.
/// @throw std::invalid_argument if the status code is invalid or values does not provide a status code and an url.
std::pair<int, std::string> ParsingHelper::parseReturn(std::vector<std::string> &values)
{
	std::pair<int, std::string> redirection;

	if (values.size() != 2)
	{
		Logger::log(ERROR, "return directive should have a status code and an url.");
		throw std::invalid_argument("Error: return directive should have a status code and an url.");
	}
	try
	{
		redirection.first = std::stoi(values[0]);
	}
	catch (const std::exception &e)
	{
		Logger::log(ERROR, "Invalid return status code.");
		throw std::invalid_argument("Error: Invalid return status code.");
	}

	redirection.second = values[1];
	return redirection;
}

/// @brief Parses string values related to the directive 'error_page'.
/// @param info std::vector where each element contains a string with data of an error page (the status codes and the url).
/// @return A map where the key is an url error page and its value a vector of status codes.
std::map<int, std::string> ParsingHelper::parseErrorPages(std::vector<std::string> &info)
{
	std::map<int, std::string> errorPageMap;

	for (std::vector<std::string>::iterator it = info.begin(); it != info.end(); it++)
	{
		parseErrorPage(errorPageMap, *it);
	}

	return errorPageMap;
}

/// @brief Parses string values of an error page.
/// @param errorPageMap Map to stored the parsed error page.
/// @param info Contains values of the error page to parsed. Should have status codes and an url separated by spaces.
/// @throw std::invalid_argument if info does not contain at least one integer representing a status code and an url.
void ParsingHelper::parseErrorPage(std::map<int, std::string> &errorPageMap, std::string &info)
{
	std::vector<std::string> values = split(info, ' ');

	if (values.size() < 2)
	{
		Logger::log(ERROR, "Invalid values in error_page directive.");
		throw std::invalid_argument("Error: Invalid values in error_page directive.");
	}

	std::string errorPageUrl = values.back();

	for (std::vector<std::string>::iterator it = values.begin(); it != values.end() - 1; it++)
	{
		try
		{
			int statusCode = std::stoi(*it);
			errorPageMap[statusCode] = errorPageUrl;
		}
		catch (const std::exception &e)
		{
			Logger::log(ERROR, "Invalid status code.");
			throw std::invalid_argument("Error: Invalid status code.");
		}
	}
}
