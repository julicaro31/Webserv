#include "ParsingHelper.hpp"
#include "ConfigBlock.hpp"

/// @brief Parses a configuration file.
/// @param configFilePath Path of the configuration file.
/// @return The parsed configuration file stored in a ConfigBlock.
ConfigBlock ParsingHelper::parseConfigFile(std::string &configFilePath)
{
	std::ifstream configFile(configFilePath);

	if (!configFile.is_open())
	{
		throw std::runtime_error("Error: Could not open file.");
	}

	ConfigBlock rootBlock = parseBlock(configFile, toString(Context::NONE));
	configFile.close();
	return rootBlock;
}

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
				throw std::runtime_error("Error: Unmatched closing brace.");
			}
		}
		else
		{
			throw std::runtime_error("Error: Incorrect syntax in file.");
		}
	}
	if (braceLevel > 0)
	{
		throw std::runtime_error("Error: Missing closing brace.");
	}
	return block;
}

void ParsingHelper::handleDirective(ConfigBlock &block, std::string blockName, std::string line)
{
	line.pop_back();
	std::stringstream ss(line);
	std::string key, value;
	ss >> key;
	getline(ss, value);
	block.addDirective(key, trim(value), blockName.substr(0, blockName.find(" ")));
}

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

/// @brief Parses a request.
/// @param strRequest Request line containing method, target's path and http version.
/// @return Request object.
Request ParsingHelper::parseRequest(std::string strRequest)
{
	std::stringstream streamStrRequest(strRequest);
	std::string method, target, version;

	streamStrRequest >> method >> target >> version;

	std::map<std::string, std::string> headers;
	std::string header;
	std::getline(streamStrRequest, header);
	while (getline(streamStrRequest, header))
	{
		if (header == "\r" || header.empty())
		{
			break;
		}

		size_t colonPos = header.find(':');
		if (colonPos != std::string::npos)
		{
			std::string headerName = header.substr(0, colonPos);
			std::string headerValue = ParsingHelper::trim(header.substr(colonPos + 1));
			headers[headerName] = headerValue;
		}
	}

	Request request(parseMethod(method), target, version, headers);

	return request;
}

Method ParsingHelper::parseMethod(std::string method)
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

std::vector<ServerConfig> ParsingHelper::createServersConfiguration(std::string &configFilePath)
{
	ConfigBlock configFile = ParsingHelper::parseConfigFile(configFilePath);
	configFile.print();

	std::vector<ServerConfig> serversConfig;
	ConfigBlock httpBlock = configFile.getConfigBlocksByContext(Context::HTTP)[0];
	std::vector<ConfigBlock> serverConfigBlocks = httpBlock.getConfigBlocksByContext(Context::SERVER);

	bool firstServer = true;
	for (std::vector<ConfigBlock>::iterator serverBlock = serverConfigBlocks.begin(); serverBlock != serverConfigBlocks.end(); serverBlock++)
	{
		bool defaultServer = firstServer;
		firstServer = false;
		bool autoIndex = false;

		std::string host;
		int port;
		size_t maxBodySize = 0;
		std::vector<std::string> index;
		std::string root;
		std::map<std::string, std::vector<int>> errorPages;

		std::map<std::string, std::vector<std::string>> &directives = serverBlock->getDirectives();

		for (std::map<std::string, std::vector<std::string>>::iterator directive = directives.begin(); directive != directives.end(); directive++)
		{
			std::string directiveName = directive->first;
			if (directiveName == "autoindex")
			{
				autoIndex = parseAutoIndex(directive->second[0]);
			}
			else if (directiveName == "client_max_body_size")
			{
				maxBodySize = parseMaxBodySize(directive->second[0]);
			}
			else if (directiveName == "listen")
			{
				std::pair<std::string, int> hostAndPort = parseHostAndPort(directive->second[0]);
				host = hostAndPort.first;
				port = hostAndPort.second;
			}
			else if (directiveName == "index")
			{
				index = directive->second;
			}
			else if (directiveName == "root")
			{
				root = directive->second[0];
			}
			else if (directiveName == "error_page")
			{
				errorPages = parseErrorPages(directive->second);
			}
		}

		std::cout << "Default server: " << defaultServer << " Host: " << host << " Port: " << port << " AutoIndex: " << autoIndex << " MaxBodySize: " << maxBodySize << " Index: " << index[0] << " Root: " << root << std::endl;

		for (const auto &pair : errorPages)
		{
			std::cout << "Key: " << pair.first << ", Values: ";

			for (const auto &val : pair.second)
			{
				std::cout << val << " ";
			}

			std::cout << std::endl;
		}
	}
	return serversConfig;
}

bool ParsingHelper::parseAutoIndex(std::string &info)
{
	if (info == "on")
	{
		return true;
	}
	if (info == "off")
	{
		return false;
	}
	throw std::runtime_error("Error: Autoindex value should be on or off.");
}

size_t ParsingHelper::parseMaxBodySize(std::string &info)
{
	try
	{
		int maxBodySize = std::stoi(info);
		if (maxBodySize >= 0)
		{
			return maxBodySize;
		}
		throw std::runtime_error("Error: client_max_body_size should be a positive integer.");
	}
	catch (const std::invalid_argument &e)
	{
		throw std::runtime_error("Error: client_max_body_size should be a positive integer.");
	}
}

std::pair<std::string, int> ParsingHelper::parseHostAndPort(std::string &info)
{
	std::vector<std::string> values = split(info, ':');
	int valuesSize = values.size();

	if (valuesSize == 1)
	{
		try
		{
			int port = std::stoi(values[0]);
			return std::make_pair("0.0.0.0", port);
		}
		catch (const std::invalid_argument &e)
		{
			return std::make_pair(values[0], 80);
		}
		catch (const std::out_of_range &e)
		{
			throw std::runtime_error("Error: Invalid port.");
		}
	}

	if (valuesSize == 2)
	{
		try
		{
			int port = std::stoi(values[1]);
			return std::make_pair(values[0], port);
		}
		catch (const std::exception &e)
		{
			throw std::runtime_error("Error: Invalid port.");
		}
	}

	throw std::runtime_error("Error: Invalid host and port.");
}

std::map<std::string, std::vector<int>> ParsingHelper::parseErrorPages(std::vector<std::string> &info)
{
	std::map<std::string, std::vector<int>> errorPageMap;

	for (std::vector<std::string>::iterator it = info.begin(); it != info.end(); it++)
	{
		parseErrorPage(errorPageMap, *it);
	}

	return errorPageMap;
}

void ParsingHelper::parseErrorPage(std::map<std::string, std::vector<int>> &errorPageMap, std::string &info)
{
	std::vector<std::string> values = split(info, ' ');

	if (values.size() < 2)
	{
		throw std::runtime_error("Error: Invalid values in error_page directive.");
	}

	std::string errorPageUrl = values.back();

	for (std::vector<std::string>::iterator it = values.begin(); it != values.end() - 1; it++)
	{
		try
		{
			errorPageMap[errorPageUrl].push_back(std::stoi(*it));
		}
		catch (const std::exception &e)
		{
			throw std::runtime_error("Error: Invalid status code.");
		}
	}
}