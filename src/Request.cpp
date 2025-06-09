#include "Request.hpp"
#include "Token.hpp"
#include "ParsingHelper.hpp"

Method Request::getMethod() const
{
	return (method);
}

float Request::getVersion() const
{
	return (version);
}

std::string Request::getUri() const
{
	return (uri);
}

std::string Request::getQueryString() const
{
	return (queryString);
}

std::unordered_map<std::string, std::string>  Request::getHeaders() const
{
	return (headers);
}

std::string Request::getBody() const
{
	return (body);
}

std::string Request::headersToString(std::unordered_map<std::string, std::string> map)
{
	std::string ret("***************\n");
	for (auto it:map)
	{
		ret.append("key: " + it.first + ", value: " + it.second + "\n");
	}
	ret.append("***************\n");
	return (ret);
}

std::ostream& operator<<(std::ostream& out, const Request& request)
{
	out << "---------------" << std::endl;
	out << "Request.method: " << ParsingHelper::methodToStr(request.getMethod()) << std::endl;
	out << "Request.version: " << std::to_string(request.getVersion()) << std::endl; 
	out	<< "Request.uri: " << request.getUri() << std::endl;
	out	<< "Request.queryString: " << request.getQueryString() << std::endl;
	out	<< "Request.headers \n" << Request::headersToString(request.getHeaders()) << std::endl;
	out	<< "Request.body: " << request.getBody() << std::endl;
	out << "---------------" << std::endl;
	return (out);
}
	
std::string Request::requestToString(const Request& request)
{
	std::string ret("\n***************\n");
	ret.append("Request.method: " + ParsingHelper::methodToStr(request.getMethod()) + "\n" );
	ret.append("Request.version: " + std::to_string(request.getVersion()) + "\n" );
	ret.append("Request.uri: " + request.getUri() + "\n" );
	ret.append("Request.queryString: " + request.getQueryString() + "\n" );
	ret.append("Request.headers \n" + headersToString(request.getHeaders()) + "\n" );
	ret.append("Request.body: " + request.getBody() + "\n" );
	ret.append("---------------\n" );
	return (ret);
}

std::string parseQueryString(const std::string& uri)
{
    std::size_t found;

    found = uri.find("?");
    if (found != std::string::npos)
        return (uri.substr(++found));
    return (std::string());
}

std::string parseUri(const std::string& uri)
{
    std::size_t found;

    found = uri.find("?");
    if (found != std::string::npos)
        return (uri.substr(0, found));
    return (std::string(uri));
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
			case Token::URI:
				uri = parseUri(it.getLiteral());
                queryString = parseQueryString(it.getLiteral());
				break;
			case Token::HEADER:
				{
					std::string header = it.getLiteral();
					std::size_t delimiter = header.find(':');
					std::string header_name = header.substr(0, delimiter);
					std::string header_value = header.substr(delimiter + 2, header.size() - 1);
					headers.insert({header_name, header_value});
				}
				break;
			case Token::BODY:
				body = it.getLiteral();
				break;
		default:
			break;
		}
	}
}

Request::Request(const Request & request)
	: method{ request.method}
	, version {request.version}
	, uri {request.uri}
	, headers {request.headers}
	, body {request.body}
	, queryString {request.queryString}
{
}

Request::~Request(void)
{
}
