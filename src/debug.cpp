#include "debug.hpp"

void debugPrint(const char * file, int line, const char * func, const std::string & message)
{
	std::cout	<< "DEBUG: File: " << file
				<< ", Line: " << line
				<< ", Func: " << func
				<< " -> " << message << std::endl;
}
