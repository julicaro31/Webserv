#include "ParsingHelper.hpp"
#include "ConfigBlock.hpp"

/// @brief Parses a configuration file.
/// @param configFilePath Path of the configuration file.
/// @return The parsed configuration file stored in a ConfigBlock.
ConfigBlock ParsingHelper::parseConfigFile(std::string& configFilePath)
{
	std::ifstream configFile(configFilePath);

	if (!configFile.is_open())
	{
		std::cerr << "Error: Could not open file " << configFilePath << std::endl;
		exit(1);
	}

	ConfigBlock rootBlock = parseBlock(configFile, toString(Context::NONE));
	configFile.close();
	return rootBlock;
}

ConfigBlock ParsingHelper::parseBlock(std::ifstream& file, std::string blockName, int braceLevel)
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
				std::cerr << "Error: Unmatched closing brace." << braceLevel << std::endl;
				exit(1);
			}
		}
		else
		{
			std::cerr << "Error: Incorrect syntax in file." << std::endl;
			exit(1);
		}
	}
	if (braceLevel > 0)
	{
		std::cerr << "Error: Missing closing brace." << std::endl;
		exit(1);
	}
	return block;
}

void ParsingHelper::handleDirective(ConfigBlock& block, std::string blockName, std::string line)
{
	line.pop_back();
	std::stringstream ss(line);
	std::string key, value;
	ss >> key;
	getline(ss, value);
	block.addDirective(key, trim(value), blockName.substr(0, blockName.find(" ")));
}

void ParsingHelper::handleSubBlock(std::ifstream &file, ConfigBlock& block, std::string blockName, std::string line)
{
	std::string subBlockName = trim(line.substr(0, line.size() - 1));
	ConfigBlock subBlock = parseBlock(file, subBlockName, 1);
	block.addSubBlock(subBlockName, blockName, subBlock);
}

/// @brief Removes outer whitespaces.
/// @param str String to trim.
/// @return The trimmed string.
std::string ParsingHelper::trim(const std::string& str)
{
	std::string::const_iterator start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) { return std::isspace(ch);});

	std::string::const_iterator end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {return std::isspace(ch);}).base();

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
		case Context::HTTP: return "http";
		case Context::SERVER: return "server";
		case Context::LOCATION: return "location";
		case Context::NONE: return "none";
		default: return "unknown";
	}
}

/// @brief Splits a string into a vector of substrings based on a specified delimiter.
/// @param str The input string to be split.
/// @param delimiter The character that separates the substrings in the input string.
/// @return A vector of strings, where each element is a substring from the original string, split at each occurrence of the delimiter.
std::vector<std::string> ParsingHelper::split(const std::string& str, char delimiter)
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