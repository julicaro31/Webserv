#ifndef __CONFIG_BLOCK_H__
#define __CONFIG_BLOCK_H__

#include "Directives.hpp"

/// @brief Class that parses the configuration file.
class ConfigBlock
{
private:
	std::map<std::string, std::vector<std::string>> _directives;
	std::map<std::string, std::vector<ConfigBlock>> _subConfigBlocks;

	void checkIfValidDirective(const std::string &key, const std::string &context, std::vector<std::string> &values);

public:
	ConfigBlock();
	~ConfigBlock();
	ConfigBlock(const ConfigBlock &configBlock);
	ConfigBlock &operator=(const ConfigBlock &configBlock);

	void addDirective(const std::string &key, const std::string &value, const std::string &context);
	void addSubBlock(const std::string &blockName, const std::string &parentBlockName, const ConfigBlock &block);
	void print(int indent = 0) const;

	std::map<std::string, std::vector<ConfigBlock>> &getSubConfigBlocks();
	std::map<std::string, std::vector<std::string>> &getDirectives();
};

#endif