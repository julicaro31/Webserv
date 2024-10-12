#include "ConfigBlock.hpp"

ConfigBlock parseConfigFile(std::string& configFilePath)
{
	std::ifstream configFile(configFilePath);

	if (!configFile.is_open())
	{
		std::cerr << "Error: Could not open file " << configFilePath << std::endl;
		exit(1);
	}

	ConfigBlock rootBlock = parseBlock(configFile);
	configFile.close();
	return rootBlock;
}

ConfigBlock parseBlock(std::ifstream& file)
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
			block.addDirective(key, trim(value));
		}
		else if (line.back() == '{')
		{
			std::string blockName = trim(line.substr(0, line.size() - 1));
			ConfigBlock subBlock = parseBlock(file);
			block.addSubBlock(blockName, subBlock);
		}
		else if (line.back() == '}')
		{
			return block;
		}
	}
	return block;
}

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

void ConfigBlock::addDirective(const std::string& key, const std::string& value) 
{
	_directives[key] = value;
}

void ConfigBlock::addSubBlock(const std::string& blockName, const ConfigBlock& block)
{
	_subConfigBlocks[blockName].push_back(block);
}

void ConfigBlock::print(int indent) const 
{
	std::string indentation(indent, ' ');
	
	for (std::map<std::string, std::string>::const_iterator it = _directives.begin(); it != _directives.end(); it++)
	{
		std::cout << indentation << it->first << " " << it->second << ";" << std::endl;
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