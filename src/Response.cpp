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
		_cgiPass = "";
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
		_cgiPass = bestMatch->cgiPass;
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

	if (isCGI())
	{
		// Handle CGI request
	}
	else if (isFile(uri))
	{
		std::string filePath = uri + "/" + uri;
		// Check if exists and return requested file
		// If not found: 404
	}
	else
	{
		for (std::vector<std::string>::const_iterator index = _index.begin(); index != _index.end(); index++)
		{
			if (isFile(*index))
			{
				std::string filePath = uri + "/" + *index;
				// Check if exists and return index file
				// If not found: 404
				return;
			}
		}
		if (_autoIndex)
		{
			// Handle autoindex
		}
		else
		{
			// Return 404 Not Found
		}
	}
}

void Response::handlePostRequest(const std::string &uri)
{
	if (!isAllowed(Method::POST) || !isFile(uri))
	{
		// Handle method not allowed (405)
		return;
	}

	if (isCGI())
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
	if (isAllowed(Method::DELETE) == false)
	{
		// Handle method not allowed (405)
		return;
	}
	if (isCGI())
	{
		// Handle CGI request
	}
	else if (isFile(uri))
	{
		// Delete file
	}
	else
	{
		// No write access: 403
		// Try delete it
	}
}

bool Response::isCGI() const
{
	return _cgiPass != "";
}

bool Response::isAllowed(Method method) const
{
	for (std::vector<LimitExceptDirective>::const_iterator limit = _limitExcepts.begin(); limit != _limitExcepts.end(); limit++)
	{
		if (std::find(limit->methods.begin(), limit->methods.end(), method) != limit->methods.end())
		{
			return true;
		}
		if (limit->deny == "all")
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
