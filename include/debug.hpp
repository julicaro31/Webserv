#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <string>
#include <iostream>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

#define BBLACK   "\033[90m"
#define BRED     "\033[91m"
#define BGREEN   "\033[92m"
#define BYELLOW  "\033[93m"
#define BBLUE    "\033[94m"
#define BMAGENTA "\033[95m"
#define BCYAN    "\033[96m"
#define BWHITE   "\033[97m"

#define BOLD      "\033[1m"
#define UNDERLINE "\033[4m"

#define DEBUG_PRINT(msg) debugPrint(__FILE__,__LINE__,__func__, msg)

void debugPrint(const char* , int , const char* , const std::string&);

#endif
