#include "ConfigBlock.hpp"

std::string ConfigBlock::trim(const std::string& str)
{
	std::string::const_iterator start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) { return std::isspace(ch);});

	std::string::const_iterator end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {return std::isspace(ch);}).base();

	if (start >= end) 
	{
		return "";
	}

	return std::string(start, end);
}

ConfigBlock ConfigBlock::parseBlock(std::ifstream& file)
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
		}
	}
}