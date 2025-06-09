#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "Token.hpp"
#include <unordered_map>
#include <vector>
#include "Method.hpp"

class Request
{
public:
	Method getMethod() const;
	float getVersion() const;
	std::string getUri() const;
	std::string getQueryString() const;
	std::unordered_map<std::string, std::string> getHeaders() const;
	std::string getBody() const;
	static std::string headersToString(std::unordered_map<std::string, std::string>);
	static std::string requestToString(const Request& request);
	const std::string& getHost();
	Request(std::vector<Token>);
	Request(const Request &);
	~Request(void);
private:
	Method method;
	float version;
	std::string uri;
	std::unordered_map<std::string, std::string> headers;
	std::string body;
	std::string queryString;
};

std::ostream& operator<<(std::ostream& out, const Request& request);

#endif
