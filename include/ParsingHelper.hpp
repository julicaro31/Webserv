#ifndef __PARSING_HELPER_H__
#define __PARSING_HELPER_H__

class ConfigBlock;

#include "Directives.hpp"
#include "Method.hpp"
#include "ServerConfig.hpp"
#include "Location.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>
#include "Logger.hpp"

class ParsingHelper
{
public:
	static std::string trim(const std::string &str);
	static std::string toString(Context context);
	static std::vector<std::string> split(const std::string &str, char delimiter);
	static std::string methodToStr(Method method);

	static std::vector<ServerConfig> getServersConfig(std::string &configFilePath);

private:
	ParsingHelper() = delete;
	ParsingHelper(const ParsingHelper &parsingHelper) = delete;
	~ParsingHelper() = delete;
	const ParsingHelper &operator=(const ParsingHelper &parsingHelper) = delete;

	static ConfigBlock parseConfigFile(std::string &configFilePath);
	static ConfigBlock parseBlock(std::ifstream &file, std::string blockName, int braceLevel = 0);
	static void handleSubBlock(std::ifstream &file, ConfigBlock &block, std::string blockName, std::string line);
	static void handleDirective(ConfigBlock &block, std::string blockName, std::string line);

	static void parseDirectives(std::map<std::string, std::vector<std::string>> &directives, ServerConfig &serverConfig);
	static void parseDirectives(std::map<std::string, std::vector<std::string>> &directives, Location &locationConfig);
	
	template <typename T>
	static void setDefaultValues(T &config);
	
	static bool parseAutoIndex(std::string &value);
	static size_t parseMaxBodySize(std::string &value);
	static std::pair<std::string, int> parseHostAndPort(std::string &info);
	static std::pair<int, std::string> parseReturn(std::vector<std::string> &values);
	static std::map<int, std::string> parseErrorPages(std::vector<std::string> &info);
	static void parseErrorPage(std::map<int, std::string> &errorPageMap, std::string &info);
	static void parseLocation(ConfigBlock &serverBlock, ServerConfig &serverConfig);
	static std::vector<LimitExceptDirective> parseLimitExcepts(ConfigBlock &locationBlock);
	static std::map<std::string, std::string> parseCgiExtensionMap(std::vector<std::string> &info);

	static Method parseMethod(std::string method);
};

#endif