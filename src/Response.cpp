#include "Response.hpp"
#include "CgiHandler.hpp"
#include "Logger.hpp"
#include <stdexcept>

Response::Response(const Request &request, const Server &server) : _request(request)
{
	setAccept();
	const std::vector<Location> &locations = server.getLocations();
	const Location *bestMatch = nullptr;
	size_t bestMatchLength = 0;

	for (std::vector<Location>::const_iterator location = locations.begin(); location != locations.end(); location++)
	{
		if (_request.getUri().find(location->uri) == 0)
		{
			if (location->modifier == "=" && location->uri == _request.getUri())
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

Response::~Response() {}

int Response::getStatus() const { return _status; }

std::string Response::getMsg() const { return _msg; }

void Response::setAccept()
{
	try
	{
		std::string accept = getHeaderValue("Accept");
		_accept = ParsingHelper::split(accept, ',');
	}
	catch (const std::exception &e)
	{
		_accept = {"*/*"};
	}
}

bool Response::isAccepted()
{
	for (std::vector<std::string>::iterator it = _accept.begin(); it != _accept.end(); it++)
	{
		std::vector<std::string> contentTypeInfo = ParsingHelper::split(*it, ';');
		if (!contentTypeInfo.empty() && (contentTypeInfo[0] == "text/html" || contentTypeInfo[0] == "*/*"))
		{
			return true;
		}
	}
	return false;
}

// Sets the status code and msg response to a Get, Post or Delete request
void Response::handleRequest()
{
	try
	{
		_clientHost = getHeaderValue("Host");
	}
	catch (const std::exception &e)
	{
		return handleResponseError(400);
	}

	if (_request.getBody().size() > _maxBodySize)
	{
		return handleResponseError(413);
	}


	if (!isAccepted())
	{
		return handleResponseError(406);
	}

	if (!_redirection.second.empty())
	{
		return handleRedirection();
	}

	if (isCGI())
	{
        handleCgiRequest();
	}
	else if (_request.getMethod() == Method::GET)
	{
		handleGetRequest();
	}
	else if (_request.getMethod() == Method::POST)
	{
		handlePostRequest();
	}
	else if (_request.getMethod() == Method::DELETE)
	{
		handleDeleteRequest();
	}
	else if (_request.getMethod() == Method::NONE)
	{
		handleResponseError(405);
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

const std::string Response::getHeaderValue(const std::string &headerName)
{
	std::unordered_map<std::string, std::string> headers = _request.getHeaders();
	std::unordered_map<std::string, std::string>::iterator it = headers.find(headerName);

	if (it == _request.getHeaders().end())
	{
		throw std::runtime_error("Missing " + headerName);
	}
	return it->second;
}

// If the error page corresponding to the status, it retrieves it. Otherwise responses the appropriate default msg.
void Response::handleResponseError(int status)
{
	_status = status;
	std::map<int, std::string>::const_iterator it = _errorPages.find(status);
	std::string content = "";
    auto searchMsg = defaultErrorMsgs.find(status);
    std::string errorMsg;
	std::string allowMethods = status == 405 ? +"\r\nAllow: " + getAllowedMethods() : "";

    if (searchMsg == defaultErrorMsgs.end())
    {
        _msg = "HTTP/1.1 " + std::to_string(500) + " " + "Internal Server Error" + "\r\nContent-Length: " + std::to_string(content.size()) + allowMethods + "\r\n\r\n" + content;
        return ;
    }
    else
    {
        errorMsg = searchMsg->second;
    }
	if (it != _errorPages.end())
	{
		try
		{
			content = getFileContent(getFullPath(_root + it->second));
		}
		catch (const std::exception &e)
		{
	        _msg = "HTTP/1.1 " + std::to_string(500) + " " + errorMsg + "\r\nContent-Length: " + std::to_string(content.size()) + allowMethods + "\r\n\r\n" + content;
            return ;
		}
	}
	_msg = "HTTP/1.1 " + std::to_string(status) + " " + errorMsg + "\r\nContent-Length: " + std::to_string(content.size()) + allowMethods + "\r\n\r\n" + content;
}

void Response::handleRedirection()
{
	_status = _redirection.first;
	std::unordered_map<int, std::string>::const_iterator redirectionIt = redirectionMsgs.find(_redirection.first);
	std::string msg = redirectionIt != redirectionMsgs.end() ? redirectionIt->second : "";
	_msg = "HTTP/1.1 " + std::to_string(_status) + " " + msg + "\r\nLocation: " + _redirection.second + "\r\nContent-Length: 0\r\n\r\n";
	return;
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

void Response::handleGetRequest()
{
	if (isAllowed(Method::GET, _clientHost) == false)
	{
		return handleResponseError(405);
	}

	std::string uri = _request.getUri();

	// Checks if the root is already in the path
	if (uri.compare(0, _root.size(), _root) == 0)
	{
		uri = uri.substr(_root.size());
	}

	std::string path = _root + uri;

	// Check if the path exists
	if (!std::filesystem::exists(getFullPath(path)))
	{
		return handleResponseError(404);
	}
	else if (std::filesystem::is_regular_file(getFullPath(path)))
	{
		handleFileServing(path);
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
			handleAutoIndex(path);
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
		for (std::filesystem::directory_iterator it(fullPath); it != std::filesystem::directory_iterator(); ++it)
		{
			std::filesystem::path entryPath = it->path();
			std::string name = entryPath.filename().string();

			if (std::filesystem::is_directory(entryPath))
			{
				name += "/";
			}

			std::string basePath = dirPath;
			if (!basePath.empty() && basePath.back() != '/')
			{
				basePath += '/';
			}

			std::string href = basePath + name;
			body += "  <li><a href=\"" + href + "\">" + name + "</a></li>\n";
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

void Response::handlePostRequest()
{
	if (!isAllowed(Method::POST, _clientHost) || !isFile(_request.getUri()))
	{
		return handleResponseError(405);
	}
	try
	{
		std::string contentType = getHeaderValue("Accept");
		if (contentType != "text/html" && contentType != "*/*")
		{
			return handleResponseError(415);
		}
	}
	catch (const std::exception &e)
	{
		return handleResponseError(400);
	}

	handleUpload();
}

void Response::handleUpload()
{
	std::ofstream out(getFullPath(_root + _request.getUri()), std::ios::binary);
	out.write(_request.getBody().data(), _request.getBody().size());
	out.close();

	_status = 200;
	_msg = "HTTP/1.1 200 OK\r\nContent-Length: 15\r\n\r\nUpload success\n";
}

void Response::handleDeleteRequest()
{
	if (isAllowed(Method::DELETE, _clientHost) == false)
	{
		return handleResponseError(405);
	}
	return handleDeletion(_root + _request.getUri());
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

void Response::handleCgiRequest()
{
    std::string cgi_content;
    std::string scriptPath = getFullPath(_root + _request.getUri());
    Logger::log(INFO, "executing CGI script at " + scriptPath);

    try
    {
        cgi_content = CgiHandler::execute(scriptPath);
    }
    catch (timeout_exception& e)
    {
        Logger::log(ERROR, e.what());
        return (handleResponseError(504));
    }
    catch (std::invalid_argument& e)
    {
        Logger::log(ERROR, e.what());
        return (handleResponseError(404));
    }
    catch (std::logic_error& e)
    {
        Logger::log(ERROR, e.what());
        return (handleResponseError(403));
    }
    catch (std::runtime_error &e)
    {
        Logger::log(ERROR, e.what());
        return (handleResponseError(500));
    }
    try
    {
        _status = 200;
        _msg = "HTTP/1.1 200 OK\r\nContent-Type: " + getMimeType(_root + _request.getUri()) + "\r\nContent-Length: " + std::to_string(cgi_content.size()) + "\r\n\r\n" + cgi_content;
    }
    catch (const std::runtime_error &e)
    {
        handleResponseError(500);
    }
}
