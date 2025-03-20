#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <string>
#include <iostream>

#define DEBUG_PRINT(msg) debugPrint(__FILE__,__LINE__,__func__, msg)

void debugPrint(const char* , int , const char* , const std::string&);

#endif
