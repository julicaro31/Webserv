#ifndef __CONFIG_BLOCK_H__
#define __CONFIG_BLOCK_H__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cctype>

class ConfigBlock
{
	private:
		
		std::map<std::string, std::string> _directives;
		std::map<std::string, std::vector<std::string, ConfigBlock>> _subConfigBlocks;

		ConfigBlock parseBlock(std::ifstream& file);
		std::string trim(const std::string& str);

	public:

		ConfigBlock();
		~ConfigBlock();
		ConfigBlock(const ConfigBlock& configBlock);
		ConfigBlock& operator=(const ConfigBlock& configBlock);
};

#endif