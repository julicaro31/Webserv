#ifndef __LIMIT_EXCEPT_DIRECTIVE_H__
#define __LIMIT_EXCEPT_DIRECTIVE_H__

#include "Method.hpp"
#include <vector>
#include <iostream>

struct LimitExceptDirective
{
	std::vector<Method> methods;
	std::string allow;
	std::string deny;
};

#endif