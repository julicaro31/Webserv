#include "Response.hpp"

Response::Response(const std::string &uri, const Server &server)
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

std::string Response::getRequestMsg(Method method, const std::string &uri, const std::string clientHost)
{
	if (method == Method::GET)
	{
		handleGetRequest(uri, clientHost);
	}
	else if (method == Method::POST)
	{
		handlePostRequest(uri, clientHost);
	}
	else if (method == Method::DELETE)
	{
		handleDeleteRequest(uri, clientHost);
	}
	else
	{
		handleBadRequest();
	}
	return _msg;
}

void Response::handleGetRequest(const std::string &uri, const std::string clientHost)
{
	if (isAllowed(Method::GET, clientHost) == false)
	{
		return handleMethodNotAllowed();
	}

	if (isCGI())
	{
		// Handle CGI request
	}
	else if (isFile(uri))
	{
		std::string filePath = _root + uri;

		handleFileServing(filePath);
	}
	else
	{
		for (std::vector<std::string>::const_iterator index = _index.begin(); index != _index.end(); index++)
		{
			if (isFile(*index))
			{
				std::string filePath = _root + "/" + *index;
				handleFileServing(filePath);
				return;
			}
		}
		if (_autoIndex)
		{
			handleAutoIndex(uri);
		}
		else
		{
			handleFileNotFound();
		}
	}
}

void Response::handlePostRequest(const std::string &uri, const std::string clientHost)
{
	if (!isAllowed(Method::POST, clientHost) || !isFile(uri))
	{
		return handleMethodNotAllowed();
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

void Response::handleDeleteRequest(const std::string &uri, const std::string clientHost)
{
	if (isAllowed(Method::DELETE, clientHost) == false)
	{
		return handleMethodNotAllowed();
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

void Response::handleBadRequest()
{
}

void Response::handleMethodNotAllowed()
{
	_msg =
		"HTTP/1.1 405 Method Not Allowed\r\n"
		"Content-Length: 0\r\n"
		"Allow: " +
		getAllowedMethods() + "\r\n"
							  "\r\n";
}

std::string Response::getAllowedMethods() const
{
	std::set<std::string> methods;

	for (std::vector<LimitExceptDirective>::const_iterator it = _limitExcepts.begin(); it != _limitExcepts.end(); it++)
	{
		for (std::vector<Method>::const_iterator method = it->methods.begin(); method != it->methods.end(); method++)
		{
			methods.insert(ParsingHelper::methodToStr(*method));
		}
	}

	std::string str = "";

	for (std::set<std::string>::const_iterator it = methods.begin(); it != methods.end(); it++)
	{
		str += *it + " ";
	}

	if (!str.empty())
	{
		str.pop_back();
	}

	return str;
}

bool Response::isAllowed(Method method, const std::string clientHost) const
{
	for (std::vector<LimitExceptDirective>::const_iterator it = _limitExcepts.begin(); it != _limitExcepts.end(); it++)
	{
		if (std::find(it->methods.begin(), it->methods.end(), method) != it->methods.end())
		{
			return true;
		}
		if (it->deny == "all")
		{
			return false;
		}

		return (it->allow == clientHost);
	}
	return true;
}

void Response::handleFileServing(const std::string &path)
{
	try
	{
		std::string content = readFile(path);
		_msg =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: " + getMimeType(path) +"\r\n"
			"Content-Length: " +
			std::to_string(content.size()) + "\r\n"
											 "\r\n" +
			content;
	}
	catch (const std::runtime_error &e)
	{
		handleFileNotFound();
	}
}

void Response::handleFileNotFound()
{
	_msg =
		"HTTP/1.1 404 Not Found\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 17\r\n"
		"\r\n"
		"File Not Found";
}

const std::string Response::readFile(const std::string &path)
{
	std::string fullPath = std::filesystem::current_path().c_str() + path;

	std::ifstream file(fullPath, std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + fullPath);
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string Response::getMimeType(const std::string& fileName)
{
	std::string fileExtension = "";
	size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos && dotPos != 0)
	{
        fileExtension = fileName.substr(dotPos);
    }
    
	std::unordered_map<std::string, std::string>::const_iterator it = mimeTypes.find(fileExtension);

	if (it != mimeTypes.end())
	{
		return it->second;
	}
	return "text/html";
}

void Response::handleAutoIndex(const std::string &dirPath)
{
    std::string body = 
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><title>Index of " + dirPath + "</title></head>\n"
		"<body>\n"
		"<h1>Index of " + dirPath + "</h1>\n"
		"<ul>\n";

	std::string fullPath = std::filesystem::current_path().c_str() + dirPath;
	try
	{
		for (const auto& entry : std::filesystem::directory_iterator(fullPath)) 
		{
      	  	std::string name = entry.path().filename().string();
       		body += "  <li><a href=\"" + name + "\">" + name + "</a></li>\n";
    	}
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		handleFileNotFound();
		return;
	}

    body += "</ul>\n</body>\n</html>\n";

    _msg = 
		"HTTP/1.1 200 OK\r\n"
    	"Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
		"\r\n" +
        body;
}

bool Response::isCGI() const
{
	return _cgiPass != "";
}

bool Response::isFile(const std::string &uri) const
{
	return uri.back() != '/';
}

void testResponse(const std::string &uri, const Server &server)
{
	Response response(uri, server);
	std::string msg = response.getRequestMsg(Method::GET, uri, "clientHost");

	std::cout << msg << std::endl;
}