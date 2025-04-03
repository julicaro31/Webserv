#include "Request.hpp"
#include "Token.hpp"
#include <utility>

Request::Method Request::getMethod(void) const
{
	return (method);
}

float Request::getVersion(void) const
{
	return (version);
}

std::string Request::getUri (void) const
{
	return (uri);
}

std::vector<std::pair<std::string, std::string>> Request::getHeaders(void) const
{
	return (headers);
}

std::string Request::getBody(void) const
{
	return (body);
}

Request::Request(std::vector<Token> tokens)
{
	for (auto it:tokens)
	{
		Token::TokenType type = it.getType();
		switch(type){
			case Token::METHOD:
				{
					std::string lexeme = it.getLexeme();
					if (lexeme == "GET")
						method = Method::GET;
					else if (lexeme == "POST")
						method = Method::POST;
					else if (lexeme == "DELETE")
						method = Method::DELETE;
					else
						method = Method::NONE;
				}
				break;
			case Token::VERSION: 
				version = std::stof(it.getLiteral());
				break;
			case Token::URI: break;
				uri = it.getLiteral();	
			case Token::HEADER: break;
				{
					std::string header = it.getLiteral();
					std::size_t delimiter = header.find(':');
					std::string header_name = header.substr(0, delimiter);
					std::string header_value = header.substr(delimiter, header.size() - 1);
					headers.push_back(std::make_pair(header_name, header_value));
				}
			case Token::BODY: break;
		default:
			break;
		}
	}
}

Request::Request(const Request & request)
{
	method = request.getMethod();
	version = request.getVersion();
	uri = request.getUri();
	headers = request.getHeaders();
	body = request.getBody();
}

Request::~Request(void)
{
}
