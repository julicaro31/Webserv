#ifndef __EXPR_H__
#define __EXPR_H__

#include <cctype>
#include "Expr.hpp"

template <typename T>
class Binary : public Expr<T>
{
public:
	T accept(Visitor<T>&) override;
	Binary(Expr<T>, Token, Expr<T>);
	~Binary();
};

#endif
