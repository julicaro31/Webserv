#include "Response.hpp"

/// Creates a Response object. Based on the uri, finds the most suitable location in the configuration file.
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
	// If there is no match, the configuration is the one outside the location blocks
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
		// Configuration in most suitable location
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
int Response::getStatus() const { return _status; }

std::string Response::getMsg() const { return _msg; }

// Sets the status code and msg response to a Get, Post or Delete request
void Response::setStatusAndMsg(Method method, const std::string &uri, const std::string &clientHost, const std::string &body)
{
	if (method == Method::GET)
	{
		handleGetRequest(uri, clientHost);
	}
	else if (method == Method::POST)
	{
		handlePostRequest(uri, clientHost, body);
	}
	else if (method == Method::DELETE)
	{
		handleDeleteRequest(uri, clientHost);
	}
	else
	{
		handleResponseError(400);
	}
}

bool Response::isCGI() const
{
	return _cgiPass != "";
}

bool Response::isFile(const std::string &uri) const
{
	return uri.back() != '/';
}

const std::string Response::getFullPath(const std::string &path)
{
	return std::filesystem::current_path().c_str() + path;
}

// Returns the content of a file in a string
const std::string Response::getFileContent(const std::string &fullPath)
{
	std::ifstream file(fullPath, std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + fullPath);
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

// If the error page corresponding to the status, it retrieves it. Otherwise responses the appropriate default msg.
void Response::handleResponseError(int status)
{
	_status = status;
	std::map<int, std::string>::const_iterator it = _errorPages.find(status);

	std::string content = "";
	if (it != _errorPages.end())
	{
		try
		{
			content = getFileContent(getFullPath(_root + it->second));
		}
		catch (const std::exception &e)
		{
		}
	}

	std::string allowMethods = status == 405 ? +"\r\nAllow: " + getAllowedMethods() : "";

	_msg = "HTTP/1.1 " + std::to_string(status) + " " + defaultErrorMsgs.find(status)->second + "\r\nContent-Length: " + std::to_string(content.size()) + allowMethods + "\r\n\r\n" + content;
}

// Gets the allowed methods by checking the limit_except directive
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

// Returns if the method is allowed by checking the limit_except directive
bool Response::isAllowed(Method method, const std::string &clientHost) const
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

void Response::handleGetRequest(const std::string &uri, const std::string &clientHost)
{
	if (isAllowed(Method::GET, clientHost) == false)
	{
		return handleResponseError(405);
	}

	if (isCGI())
	{
		// Handle CGI request
	}
	else if (isFile(uri))
	{
		handleFileServing(_root + uri);
	}
	else
	{
		// If the uri is a folder, it will serve an index (if exists)
		for (std::vector<std::string>::const_iterator index = _index.begin(); index != _index.end(); index++)
		{
			if (isFile(*index))
			{
				return handleFileServing(_root + "/" + *index);
			}
		}
		if (_autoIndex)
		{
			handleAutoIndex(uri);
		}
		else
		{
			handleResponseError(404);
		}
	}
}

void Response::handleFileServing(const std::string &path)
{
	std::string fullPath = getFullPath(path);
	std::filesystem::file_status fileStatus = std::filesystem::status(fullPath);

	// Check if file exists
	if (!std::filesystem::exists(fileStatus) || !std::filesystem::is_regular_file(fileStatus))
	{
		return handleResponseError(404);
	}

	// Check permissions
	if ((fileStatus.permissions() & std::filesystem::perms::others_read) == std::filesystem::perms::none)
	{
		return handleResponseError(403);
	}

	try
	{
		const std::string content = getFileContent(fullPath);
		_status = 200;
		_msg = "HTTP/1.1 200 OK\r\nContent-Type: " + getMimeType(path) + "\r\nContent-Length: " + std::to_string(content.size()) + "\r\n\r\n" + content;
	}
	catch (const std::runtime_error &e)
	{
		handleResponseError(500);
	}
}

std::string Response::getMimeType(const std::string &fileName)
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

// Lists all the directories and files in the directory path given
void Response::handleAutoIndex(const std::string &dirPath)
{
	std::string body = "<!DOCTYPE html>\n<html>\n<head><title>Index of " + dirPath + "</title></head>\n<body>\n<h1>Index of " + dirPath + "</h1>\n<ul>\n";

	std::string fullPath = std::filesystem::current_path().c_str() + dirPath;
	try
	{
		for (const auto &entry : std::filesystem::directory_iterator(fullPath))
		{
			std::string name = entry.path().filename().string();
			body += "  <li><a href=\"" + name + "\">" + name + "</a></li>\n";
		}
	}
	catch (const std::filesystem::filesystem_error &e)
	{
		handleResponseError(404);
		return;
	}

	body += "</ul>\n</body>\n</html>\n";

	_status = 200;
	_msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;
}

void Response::handlePostRequest(const std::string &uri, const std::string &clientHost, const std::string &body)
{
	if (!isAllowed(Method::POST, clientHost) || !isFile(uri))
	{
		return handleResponseError(405);
	}

	if (isCGI())
	{
		// Handle CGI request
	}
	else
	{
		handleUpload(uri, body);
	}
}

void Response::handleUpload(const std::string &uri, const std::string &body)
{
	std::ofstream out(getFullPath(_root + uri), std::ios::binary);
	out.write(body.data(), body.size());
	out.close();

	_status = 200;
	_msg = "HTTP/1.1 200 OK\r\nContent-Length: 15\r\n\r\nUpload success\n";
}

void Response::handleDeleteRequest(const std::string &uri, const std::string &clientHost)
{
	if (isAllowed(Method::DELETE, clientHost) == false)
	{
		return handleResponseError(405);
	}
	if (isCGI())
	{
		// Handle CGI request
	}
	else
	{
		return handleDeletion(_root + uri);
	}
}

void Response::handleDeletion(const std::string &path)
{
	std::string fullPath = getFullPath(path);
	std::filesystem::file_status fileStatus = std::filesystem::status(fullPath);

	// Check if file exists
	if (!std::filesystem::exists(fileStatus) || std::filesystem::is_directory(fileStatus))
	{
		return handleResponseError(404);
	}

	// Check permissions
	if ((std::filesystem::perms::others_write) == std::filesystem::perms::none)
	{
		return handleResponseError(403);
	}

	if (std::filesystem::remove(fullPath))
	{
		_status = 200;
		_msg = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nFile deleted\n";
	}
	else
	{
		handleResponseError(500);
	}
}

// A method to test the response, must delete later
void testResponse(const std::string &uri, const Server &server)
{
	Response response(uri, server);
	response.setStatusAndMsg(Method::DELETE, uri, "clientHost", "");

	std::cout << "Status: " << std::to_string(response.getStatus()) << std::endl
			  << response.getMsg() << std::endl;
}