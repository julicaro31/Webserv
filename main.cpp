#include <iostream>
#include "ConfigBlock.hpp"
#include "Request.hpp"

int main(int ac, char *argv[])
{
	if (ac != 2)
	{
		std::cout << "Wrong number of arguments." << std::endl;
		return -1;
	}

	std::string filePath(argv[1]);
	ConfigBlock configFile = parseConfigFile(filePath);
	configFile.print();

	std::string line = "";

	Request request = parseRequest(line);

	return 0;
}