#ifndef __PARSING_HELPER_H__
#define __PARSING_HELPER_H__

class ConfigBlock;

#include "Directives.hpp"
#include "Request.hpp"
#include "Method.hpp"
#include "ServerConfig.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

class ParsingHelper
{
public:
	static std::string trim(const std::string &str);
	static std::string toString(Context context);
	static std::vector<std::string> split(const std::string &str, char delimiter);

	static std::vector<ServerConfig> getServersConfig(std::string &configFilePath);

	static Request parseRequest(std::string strRequest);

private:
	ParsingHelper();
	ParsingHelper(const ParsingHelper &parsingHelper);

	static ConfigBlock parseConfigFile(std::string &configFilePath);
	static ConfigBlock parseBlock(std::ifstream &file, std::string blockName, int braceLevel = 0);
	static void handleSubBlock(std::ifstream &file, ConfigBlock &block, std::string blockName, std::string line);
	static void handleDirective(ConfigBlock &block, std::string blockName, std::string line);

	static bool parseAutoIndex(std::string &info);
	static size_t parseMaxBodySize(std::string &info);
	static std::pair<std::string, int> parseHostAndPort(std::string &info);
	static std::map<std::string, std::vector<int>> parseErrorPages(std::vector<std::string> &info);
	static void parseErrorPage(std::map<std::string, std::vector<int>> &errorPageMap, std::string &info);

	static Method parseMethod(std::string method);
};

#endif