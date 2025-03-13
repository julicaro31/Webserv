#ifndef __EXPR_H__
#define __EXPR_H__

#include "Binary.hpp"
#include <cctype>
#include <Token.hpp>

class Binary;

template <typename T>
class Visitor
{
public:
	virtual T visit(T&) = 0;
	virtual ~Visitor() = default;
};

template <typename T>
class Expr
{
public:
	virtual T accept(Visitor<T>&) const = 0;
	virtual	~Expr() = default;
private:
	Expr	left;
	Token	operator;
	Expr	right;
};

#endif
