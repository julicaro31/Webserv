#ifndef __EXPR_H__
#define __EXPR_H__

#include <cctype>
#include <Token.hpp>

class Binary;

class Expr
{
	template <typename T>
	class Visitor
	{
	public:
		virtual T visitBinary(Binary) = 0;
		virtual ~Visitor() = default;
	};

	class Binary : public Expr
	{
	public:
		template <typename T>
		T accept(Visitor<T>&) override;
		Binary(Expr<T>, Token, Expr<T>);
		~Binary();
	};
public:
	template <typename T>
	virtual T accept(Visitor<T>&) const = 0;
	virtual	~Expr() = default;
private:
	Expr	left;
	Token	operator;
	Expr	right;
};

#endif
