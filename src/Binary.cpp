#include "Binary.hpp"

Binary::Binary (Expr left, Token operator, Expr right)
{
	this->left = left;
	this->operator = operator;
	this->right = right;
}
