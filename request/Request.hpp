#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "Method.hpp"
#include <iostream>

class Request
{
	private:

		Method _method;
		std::string _path;
};

#endif