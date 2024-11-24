#include "Request.hpp"

Request::Request(Method method, std::string target, std::string version, std::map<std::string, std::string> headers) : _method(method), _target(target), _version(version), _headers(headers) {}

/// @brief Parses a request.
/// @param strRequest Request line containing method, target's path and http version.
/// @return Request object.
Request parseRequest(std::string strRequest)
{
	std::stringstream ss(strRequest);
	std::string method, target, version;

	ss >> method >> target >> version;

	std::map<std::string, std::string> headers;
	std::string line;

	while (getline(ss, line))
	{
		// TODO: create map of headers.
	}

	Request request(parseMethod(method), target, version, headers);

	return request;
}

Method parseMethod(std::string method)
{
	if (method == "GET")
	{
		return Method::GET;
	}
	if (method == "POST")
	{
		return Method::POST;
	}
	if (method == "DELETE")
	{
		return Method::DELETE;
	}
	return Method::NONE;
}
