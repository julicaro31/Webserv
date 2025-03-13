#ifndef __EXPR_H__
#define __EXPR_H__

#include "Binary.hpp"
#include <cctype>
#include <Token.hpp>

class Binary;

class Visitor
{
public:
	virtual void visit(Binary& binary) = 0;
	virtual ~Visitor() = default;
};

class Expr
{
public:
	virtual void accept(Visitor&) = 0;
	virtual	~Expr() = default;
private:
	Expr	left;
	Token	operator;
	Expr	right;
};

#endif
