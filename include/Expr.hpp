#ifndef __EXPR_H__
#define __EXPR_H__

#include <cctype>
#include <Token.hpp>

class Expr
{
private:
	Expr	left;
	Token	operator;
	Expr	right;
};

#endif
