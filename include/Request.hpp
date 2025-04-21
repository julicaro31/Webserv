#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "Token.hpp"
#include <vector>

class Request
{
public:
	enum class Method
	{
		NONE,
		GET,
		POST,
		DELETE,
	};
	Method getMethod();
	float getVersion();
	std::string getUri();
	std::vector<std::pair<std::string, std::string>> getHeaders();
	std::string getBody();
	Request(std::vector<Token>);
	Request(const Request &);
	~Request(void);
private:
	Method method;
	float version;
	std::string uri;
	std::vector<std::pair<std::string, std::string>> headers;
	std::string body;
};


#endif
