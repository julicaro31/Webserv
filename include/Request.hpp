#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "Method.hpp"
#include <iostream>
#include <map>

class Request
{
	private:

		Method _method;
		std::string _target;
		std::string _version;
		std::map<std::string, std::string> _headers;

	public:

		Request(Method method, std::string target, std::string version, std::map<std::string, std::string> headers);
		Method getMethod() const;
};

#endif