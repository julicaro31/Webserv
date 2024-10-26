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

#include "Directives.hpp"

class ConfigBlock
{
	private:
		
		std::map<std::string, std::vector<std::string>> _directives;
		std::map<std::string, std::vector<ConfigBlock>> _subConfigBlocks;

	public:

		ConfigBlock();
		~ConfigBlock();
		ConfigBlock(const ConfigBlock& configBlock);
		ConfigBlock& operator=(const ConfigBlock& configBlock);

		void addDirective(const std::string& key, const std::string& value, const std::string& context);
		void addSubBlock(const std::string& blockName, const std::string& parentBlockName, const ConfigBlock& block);
		void print(int indent = 0) const;
};

	ConfigBlock parseConfigFile(std::string& configFilePath);
	ConfigBlock parseBlock(std::ifstream& file, std::string blockName = "none", bool braceOpen = false);
	std::string trim(const std::string& str);
	std::string toString(Context context);

#endif