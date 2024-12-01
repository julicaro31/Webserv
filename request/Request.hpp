#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "Method.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include "ConfigBlock.hpp" // this is included to use trim. TODO: Maybe it is better to put the function in a helper class.

class Request
{
	private:

		Method _method;
		std::string _target;
		std::string _version;
		std::map<std::string, std::string> _headers;

	public:

		Request(Method method, std::string target, std::string version, std::map<std::string, std::string> headers);
};

Request parseRequest(std::string strRequest);
Method parseMethod(std::string method);

#endif