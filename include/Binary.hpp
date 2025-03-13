#ifndef __EXPR_H__
#define __EXPR_H__

#include <cctype>
#include "Expr.hpp"

class Binary : public Expr
{
public:
	void accept(ExprVisitor& visitor) override;
	Binary(Expr, Token, Expr);
	~Binary();
};

#endif
