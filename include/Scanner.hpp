#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "Token.hpp"
#include <cctype>
#include <vector>

class Scanner
{
public:
	std::vector<Token> scanTokens(void);
private:
	Scanner();
	Scanner(const Scanner &);
};

#endif
