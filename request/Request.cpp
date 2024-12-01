#include "Request.hpp"

Request::Request(Method method, std::string target, std::string version, std::map<std::string, std::string> headers) : _method(method), _target(target), _version(version), _headers(headers) {}

/// @brief Parses a request.
/// @param strRequest Request line containing method, target's path and http version.
/// @return Request object.
Request parseRequest(std::string strRequest)
{
	std::stringstream streamStrRequest(strRequest);
	std::string method, target, version;

	streamStrRequest >> method >> target >> version;

	std::map<std::string, std::string> headers;
	std::string header;
	std::getline(streamStrRequest, header);
	while (getline(streamStrRequest, header))
	{
		if (header == "\r" || header.empty())
		{
			break;
		}
		
		size_t colonPos = header.find(':');
		if (colonPos != std::string::npos) 
		{
			std::string headerName = header.substr(0, colonPos);
			std::string headerValue = trim(header.substr(colonPos + 1));
			headers[headerName] = headerValue;
		}
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
