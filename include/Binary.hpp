#ifndef __EXPR_H__
#define __EXPR_H__

#include <cctype>
#include "Expr.hpp"

class Binary : public Expr
{
public:
	Binary(Expr, Token, Expr);
};

#endif
