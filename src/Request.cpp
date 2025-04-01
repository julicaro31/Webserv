#include "Request.hpp"
#include "Token.hpp"

Request::Request(std::vector<Token> tokens)
{
	for (auto it:tokens)
	{
		Token::TokenType type = it.getType();
		switch(type){
			case Token::METHOD: break;
			case Token::VERSION: break;
			case Token::URI: break;
			case Token::HEADER: break;
			case Token::BODY: break;
		default:

		}
	}
}

~Request(void)
{
}
