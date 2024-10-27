#ifndef __DIRECTIVES_H__
#define __DIRECTIVES_H__

#include <map>
#include <vector>
#include <iostream>
#include "Context.hpp"

extern const std::map<std::string, std::vector<Context>> Directives;
extern const std::vector<std::string> SingleUseDirectives;
extern const std::vector<std::string> SingleValueDirectives;

#endif