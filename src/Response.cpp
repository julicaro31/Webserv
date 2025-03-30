#include "Response.hpp"

void Response::setConfiguration(const std::string uri, Server &server)
{
	const std::vector<Location> &locations = server.getLocations();
	const Location *bestMatch = nullptr;
	size_t bestMatchLength = 0;

	for (std::vector<Location>::const_iterator location = locations.begin(); location != locations.end(); location++)
	{
		if (uri.find(location->uri) == 0)
		{
			if (location->modifier == "=" && location->uri == uri)
			{
				bestMatch = &(*location);
				break;
			}
			if (location->modifier == "" && location->uri.length() > bestMatchLength)
			{
				bestMatch = &(*location);
				bestMatchLength = location->uri.length();
			}
		}
	}

	if (bestMatch == nullptr)
	{
		_autoIndex = server.isAutoIndexEnabled();
		_maxBodySize = server.getMaxBodySize();
		_root = server.getRoot();
		_redirection = server.getRedirection();
		_errorPages = server.getErrorPages();
		_index = server.getIndex();
		_cgiExtensionMap = std::map<std::string, std::string>();
		_limitExcepts = {};
	}
	else
	{
		_autoIndex = bestMatch->autoIndex;
		_maxBodySize = bestMatch->maxBodySize;
		_root = bestMatch->root;
		_redirection = bestMatch->redirection;
		_errorPages = bestMatch->errorPages;
		_index = bestMatch->index;
		_cgiExtensionMap = bestMatch->cgiExtensionMap;
		_limitExcepts = bestMatch->limitExcepts;
	}
}

void Response::handleGetRequest(const std::string &uri)
{
	if (isAllowed(Method::GET) == false)
	{
		// Handle method not allowed (405)
		return;
	}

	if (isCGI(uri))
	{
		// Handle CGI request
	}
	else if (isFile(uri))
	{
		std::string filePath = uri + "/" + uri;
		// Check if exists and serve file
	}
	else
	{
		for (std::vector<std::string>::const_iterator index = _index.begin(); index != _index.end(); index++)
		{
			if (isFile(*index))
			{
				std::string filePath = uri + "/" + *index;
				// Check if exists and serve file
				return;
			}
		}
		if (_autoIndex)
		{
			// Handle autoindex
		}
		else
		{
			// Handle 404 Not Found
		}
	}
}

void Response::handlePostRequest(const std::string &uri)
{
	if (isAllowed(Method::POST) == false)
	{
		// Handle method not allowed (405)
		return;
	}

	if (isCGI(uri))
	{
		// Handle CGI request
	}
	else
	{
		// Handle upload
	}
}

void Response::handleDeleteRequest(const std::string &uri)
{
	if (isAllowed(Method::DELETE) == false || isCGI(uri))
	{
		// Handle method not allowed (405)
		return;
	}
	else
	{
		// Handle delete file or folder
	}
}

bool Response::isCGI(const std::string &uri) const
{
	const size_t extensionPos = uri.find_last_of('.');
	if (extensionPos == std::string::npos)
	{
		return false;
	}
	std::string extension = uri.substr(extensionPos);
	if (_cgiExtensionMap.find(extension) != _cgiExtensionMap.end())
	{
		return true;
	}
	return false;
}

bool Response::isAllowed(Method method) const
{
	for (std::vector<LimitExceptDirective>::const_iterator limit = _limitExcepts.begin(); limit != _limitExcepts.end(); limit++)
	{
		if (limit->method == method)
		{
			return true;
		}
		else if (limit->deny == "all")
		{
			return false;
		}
		// return (limit->allow == host);
	}
	return true;
}

bool Response::isFile(const std::string &uri) const
{
	return uri.back() != '/';
}
