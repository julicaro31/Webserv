#ifndef __DIRECTIVE_INFO_H__
#define __DIRECTIVE_INFO_H__

#include <set>
#include "Context.hpp"

struct DirectiveInfo
{
	std::set<Context> validContexts;
	std::string valueType;
};

#endif