#include "debug.hpp"
#include <string>

std::string formatString(const std::string &field, int n)
{
	if ((int)field.length() > n)
		return ("." + field.substr(field.length() - n, field.length()));
	else
	{
		return (field + std::string(n - field.length(), ' '));
	}
	return (field);
}

void debugPrint(const char * file, int line, const char * func, const std::string & message)
{
	std::cout	
				<< "["
				<< "file: " BYELLOW << formatString(file, 15)
				<< RESET
				<< " - " 
				<< "line: " GREEN << formatString(std::to_string(line), 4)
				<< RESET
				<< " - " 
				<< "func: " MAGENTA << formatString(func, 10)
				<< RESET
				<< "]"
				<< " -> " CYAN
				<< message
				<< RESET
				<< std::endl;
}

