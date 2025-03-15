#include "ConfigBlock.hpp"
#include "ParsingHelper.hpp"

ConfigBlock::ConfigBlock() {}
ConfigBlock::ConfigBlock(std::map<std::string, std::vector<std::string>> directives) : _directives(directives) {};
ConfigBlock::~ConfigBlock() {}
ConfigBlock::ConfigBlock(const ConfigBlock &configBlock) : _directives(configBlock._directives), _subConfigBlocks(configBlock._subConfigBlocks) {}

ConfigBlock &ConfigBlock::operator=(const ConfigBlock &configBlock)
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
void ConfigBlock::addDirective(const std::string &key, const std::string &value, const std::string &context)
{
	std::vector<std::string> values = ParsingHelper::split(value, ' ');

	checkIfValidDirective(key, context, values);

	if (key == "error_page" || key == "cgi_assign")
	{
		_directives[key].push_back(value);
		return;
	}

	for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); it++)
	{
		_directives[key].push_back(*it);
	}
}

/// @brief Checks if a directive is in the right context or if its value is valid.
/// @param key Name of the directive to check.
/// @param context Name of context where the directive is inside.
/// @param values Values related to the directive.
/// @throw std::runtime_error if the directive is not valid.
void ConfigBlock::checkIfValidDirective(const std::string &key, const std::string &context, std::vector<std::string> &values)
{
	if (Directives.find(key) == Directives.end())
	{
		Logger::log(ERROR, "Invalid directive: " + key);
		throw std::runtime_error("Error: Invalid directive: " + key + ".");
	}

	if (std::find(SingleUseDirectives.begin(), SingleUseDirectives.end(), key) != SingleUseDirectives.end() && !_directives[key].empty())
	{
		Logger::log(ERROR, key + " cannot appear more than once in a block.");
		throw std::runtime_error("Error: " + key + " cannot appear more than once in a block.");
	}

	if (values.empty())
	{
		Logger::log(ERROR, key + " is empty.");
		throw std::runtime_error("Error: " + key + " is empty.");
	}

	if (std::find(SingleValueDirectives.begin(), SingleValueDirectives.end(), key) != SingleValueDirectives.end() && values.size() != 1)
	{
		Logger::log(ERROR, key + " should only have one value.");
		throw std::runtime_error("Error: " + key + " should only have one value.");
	}

	std::vector<Context> possibleContexts = Directives.at(key);
	for (std::vector<Context>::iterator it = possibleContexts.begin(); it != possibleContexts.end(); it++)
	{
		if (ParsingHelper::toString(*it) == context)
		{
			return;
		}
	}
	Logger::log(ERROR, "Directive " + key + " should not be in context " + context + ".");
	throw std::runtime_error("Error: Directive " + key + " should not be in context " + context + ".");
}

/// @brief Adds a sub-block to the ConfigBlock's _subConfigBlocks.
/// @param blockName Name of the sub-block to add.
/// @param parentBlockName Parent sub-block's name.
/// @param block The sub-block to add.
/// @throw std::runtime_error if the name of the sub-block is not valid or is not inside the right block.
void ConfigBlock::addSubBlock(const std::string &blockName, const std::string &parentBlockName, const ConfigBlock &block)
{
	if (blockName == ParsingHelper::toString(Context::HTTP) && _subConfigBlocks[blockName].size() > 0)
	{
		Logger::log(ERROR, "Http block should only appear once in the configuration file.");
		throw std::runtime_error("Error: Http block should only appear once in the configuration file.");
	}

	if ((blockName == ParsingHelper::toString(Context::HTTP) && parentBlockName == ParsingHelper::toString(Context::NONE)) ||
		(blockName == ParsingHelper::toString(Context::SERVER) && parentBlockName == ParsingHelper::toString(Context::HTTP)) ||
		(blockName.substr(0, blockName.find(" ")) == ParsingHelper::toString(Context::LOCATION) && parentBlockName == ParsingHelper::toString(Context::SERVER)) ||
		(blockName.substr(0, blockName.find(" ")) == ParsingHelper::toString(Context::LIMIT_EXCEPT) && parentBlockName.substr(0, parentBlockName.find(" ")) == ParsingHelper::toString(Context::LOCATION)))
	{
		_subConfigBlocks[blockName].push_back(block);
	}
	else
	{
		Logger::log(ERROR, "Incorrect syntax in file related to block names: " + blockName);
		throw std::runtime_error("Error: Incorrect syntax in file related to block names: " + blockName);
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
	for (std::map<std::string, std::vector<ConfigBlock>>::const_iterator it = _subConfigBlocks.begin(); it != _subConfigBlocks.end(); it++)
	{
		for (std::vector<ConfigBlock>::const_iterator it2 = (it->second).begin(); it2 != (it->second).end(); it2++)
		{
			std::cout << indentation << it->first << " {" << std::endl;
			it2->print(indent + 4);
			std::cout << indentation << "}" << std::endl;
		}
	}
}

std::map<std::string, std::vector<ConfigBlock>> &ConfigBlock::getSubConfigBlocks()
{
	return this->_subConfigBlocks;
}

std::map<std::string, std::vector<std::string>> &ConfigBlock::getDirectives()
{
	return this->_directives;
}

/// @brief Gets the configBlocks representing the given contex.
/// @param context Name of the block's context to get.
/// @return Vector of ConfigBlocks, whose names match the given context.
/// @throw std::runtime_error if the context is not found.
std::vector<ConfigBlock> ConfigBlock::getConfigBlocksByContext(Context context)
{
	std::string contextStr = ParsingHelper::toString(context);
	std::map<std::string, std::vector<ConfigBlock>> configBlocks = this->getSubConfigBlocks();
	std::map<std::string, std::vector<ConfigBlock>>::iterator it = configBlocks.find(contextStr);
	if (it == configBlocks.end())
	{
		Logger::log(ERROR, contextStr + " block has not been found in the configuration file.");
		throw std::runtime_error("Error: " + contextStr + " block has not been found in the configuration file.");
	}
	return it->second;
}