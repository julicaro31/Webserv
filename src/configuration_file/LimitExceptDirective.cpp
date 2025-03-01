#include "LimitExceptDirective.hpp"

LimitExceptDirective::LimitExceptDirective(std::vector<Method> methods, std::string allow, std::string deny) : _methods{methods}, _allow{allow}, _deny{deny} {}

const std::vector<Method> LimitExceptDirective::getMethods() const
{
	return _methods;
}

const std::string LimitExceptDirective::getAllow() const
{
	return _allow;
}

const std::string LimitExceptDirective::getDeny() const
{
	return _deny;
}