#include "Binary.hpp"

void Binary::accept(ExprVisitor& visitor) override
{
	visitor.visitBinary(*this);
}

Binary::Binary (Expr left, Token operator, Expr right)
{
	this->left = left;
	this->operator = operator;
	this->right = right;
}

Binary::~Binary(void)
{
}
