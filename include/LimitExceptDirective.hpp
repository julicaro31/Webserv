#ifndef __LIMIT_EXCEPT_DIRECTIVE_H__
#define __LIMIT_EXCEPT_DIRECTIVE_H__

#include "Method.hpp"
#include <vector>
#include <iostream>

class LimitExceptDirective
{
public:
	LimitExceptDirective(std::vector<Method> methods, std::string allow, std::string deny);

	const std::vector<Method> getMethods() const;
	const std::string getAllow() const;
	const std::string getDeny() const;

private:
	std::vector<Method> _methods;
	std::string _allow;
	std::string _deny;
};

#endif