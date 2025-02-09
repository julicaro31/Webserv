#ifndef __PARSING_HELPER_H__
#define __PARSING_HELPER_H__

class ConfigBlock;

#include "Directives.hpp"
#include "Request.hpp"
#include "Method.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <cctype>

class ParsingHelper
{
	public:
	
		static ConfigBlock parseConfigFile(std::string& configFilePath);
		
		static std::string trim(const std::string& str);
		static std::string toString(Context context);
		static std::vector<std::string> split(const std::string& str, char delimiter);

		static Request parseRequest(std::string strRequest);

	private:

		ParsingHelper();
		ParsingHelper(const ParsingHelper& parsingHelper);

		static ConfigBlock parseBlock(std::ifstream& file, std::string blockName, int braceLevel = 0);
		static void handleSubBlock(std::ifstream &file, ConfigBlock& block, std::string blockName, std::string line);
		static void handleDirective(ConfigBlock& block, std::string blockName, std::string line);

		static Method parseMethod(std::string method);

};

#endif