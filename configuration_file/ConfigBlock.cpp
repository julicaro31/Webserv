#include "ConfigBlock.hpp"

ConfigBlock::ConfigBlock(){}
ConfigBlock::~ConfigBlock(){}
ConfigBlock::ConfigBlock(const ConfigBlock& configBlock): _directives(configBlock._directives), _subConfigBlocks(configBlock._subConfigBlocks) {}

ConfigBlock& ConfigBlock::operator=(const ConfigBlock& configBlock)
{
	if (this != &configBlock)
	{
		_directives = configBlock._directives;
		_subConfigBlocks = configBlock._subConfigBlocks;
	}
	return *this;
}

/// @brief Adds a directive to the block's map directives.
/// @param key Name of the directive.
/// @param value Info about the directive.
/// @param context Context of the directive.
void ConfigBlock::addDirective(const std::string& key, const std::string& value, const std::string& context) 
{
	std::vector<std::string> values = split(value, ' ');

	checkIfValidDirective(key, context, values);

	if (key == "server_name")
	{
		_directives[key].push_back(values.back());
		return;
	}

	for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); it++)
	{
		_directives[key].push_back(*it);
	}
}

void ConfigBlock::checkIfValidDirective(const std::string& key, const std::string& context, std::vector<std::string>& values)
{
	if (Directives.find(key) == Directives.end())
	{
		std::cerr << "Error: Invalid directive: " << key << "." << std::endl;
		exit(1);
	}

	if (std::find(SingleUseDirectives.begin(), SingleUseDirectives.end(), key) != SingleUseDirectives.end() && !_directives[key].empty())
	{
		std::cerr << "Error: " << key << " cannot appear more than once in a block." << std::endl;
		exit(1);
	}

	if (values.empty())
	{
		std::cerr << "Error: " << key << " is empty." << std::endl;
		exit(1);
	}

	if (std::find(SingleValueDirectives.begin(), SingleValueDirectives.end(), key) != SingleValueDirectives.end() && values.size() != 1)
	{
		std::cerr << "Error: " << key << " should only have one value." << std::endl;
		exit(1);
	}

	std::vector<Context> possibleContexts = Directives.at(key);
	for (std::vector<Context>::iterator it = possibleContexts.begin(); it != possibleContexts.end(); it++)
	{
		if (toString(*it) == context)
		{
			return;
		}
	}

	std::cerr << "Error: Directive " << key << " should not be in context " << context << "." << std::endl;
	exit(1);
}

void ConfigBlock::addSubBlock(const std::string& blockName, const std::string& parentBlockName, const ConfigBlock& block)
{
	if ((blockName == toString(Context::HTTP) && parentBlockName == toString(Context::NONE)) || 
		(blockName == toString(Context::SERVER) && parentBlockName == toString(Context::HTTP)) || 
		(blockName.substr(0, blockName.find(" ")) == toString(Context::LOCATION) && parentBlockName == toString(Context::SERVER)))
	{
		_subConfigBlocks[blockName].push_back(block);
	}
	else
	{
		std::cerr << "Error: Incorrect syntax in file related to block names." << std::endl;
		exit(1);
	}

}

void ConfigBlock::print(int indent) const 
{
	std::string indentation(indent, ' ');
	
	for (std::map<std::string, std::vector<std::string>>::const_iterator it = _directives.begin(); it != _directives.end(); it++)
	{
		std::cout << indentation << it->first;

		for (std::vector<std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			std::cout << " " << *it2;
		}
		std::cout << ";" << std::endl;
	}
	for (std::map<std::string, std::vector<ConfigBlock>>::const_iterator it = _subConfigBlocks.begin(); it!= _subConfigBlocks.end(); it++)
	{
		for (std::vector<ConfigBlock>::const_iterator it2 = (it->second).begin(); it2 != (it->second).end(); it2++)
		{
			std::cout << indentation << it->first << " {" << std::endl;
			it2->print(indent + 4);
			std::cout << indentation << "}" << std::endl;
		}
	}
}

std::map<std::string, std::vector<ConfigBlock>>& ConfigBlock::getSubConfigBlocks()
{
	return this->_subConfigBlocks;
}

std::map<std::string, std::vector<std::string>>& ConfigBlock::getDirectives()
{
	return this->_directives;
}

ConfigBlock parseConfigFile(std::string& configFilePath)
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

ConfigBlock parseBlock(std::ifstream& file, std::string blockName, bool braceOpen)
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
		else if (line.back() == ';')
		{
			line.pop_back();

			std::stringstream ss(line);
			std::string key, value;
			ss >> key;
			getline(ss, value);
			block.addDirective(key, trim(value), blockName.substr(0, blockName.find(" ")));
		}
		else if (line.back() == '{')
		{
			std::string subBlockName = trim(line.substr(0, line.size() - 1));
			ConfigBlock subBlock = parseBlock(file, subBlockName, true);
			block.addSubBlock(subBlockName, blockName, subBlock);
		}
		else if (line.back() == '}' && trim(line).size() == 1)
		{
			braceOpen = false;
			return block;
		}
		else
		{
			std::cerr << "Error: Incorrect syntax in file." << std::endl;
			exit(1);
		}
	}
	if (braceOpen)
	{
		std::cerr << "Error: Incorrect braces in file." << std::endl;
		exit(1);
	}
	return block;
}

/// @brief Removes outer whitespaces.
/// @param str String to trim.
/// @return The trimmed string.
std::string trim(const std::string& str)
{
	std::string::const_iterator start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) { return std::isspace(ch);});

	std::string::const_iterator end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {return std::isspace(ch);}).base();

	if (start >= end) 
	{
		return "";
	}

	return std::string(start, end);
}

std::string toString(Context context) 
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

std::vector<std::string> split(const std::string& str, char delimiter)
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