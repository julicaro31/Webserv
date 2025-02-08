#include "Request.hpp"

Request::Request(Method method, std::string target, std::string version, std::map<std::string, std::string> headers) : _method(method), _target(target), _version(version), _headers(headers) {}

Method Request::getMethod() const
{
	return _method;
}
