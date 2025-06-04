#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "Server.hpp"
#include "ParsingHelper.hpp"
#include "Request.hpp"
#include <set>
#include <filesystem>
#include <unordered_map>
#include <string>

class Response
{
public:
	Response(const Request &request, const Server &server);
	~Response();

	void handleRequest();
	int getStatus() const;
	std::string getMsg() const;

private:
	int _status;
	std::string _msg;
	Request _request;
	std::string _clientHost;
	std::vector<std::string> _accept;

	void setAccept();
	bool isAccepted();

	bool isCGI() const;
	bool isFile(const std::string &uri) const;
	const std::string getFullPath(const std::string &path);
	const std::string getFileContent(const std::string &fullPath);
	const std::string getHeaderValue(const std::string &headerName);

	// Error msgs
	void handleResponseError(int status);
	std::string getAllowedMethods() const;
	bool isAllowed(Method method, const std::string &clientHost) const;

	// Redirection
	void handleRedirection();

	// Get request
	void handleGetRequest();
	void handleFileServing(const std::string &path);
	std::string getMimeType(const std::string &fileName);
	void handleAutoIndex(const std::string &path);

	// Post request
	void handlePostRequest();
	void handleUpload();

	// Delete request
	void handleDeleteRequest();
	void handleDeletion(const std::string &path);

    // Cgi Request
    void handleCgiRequest();

	bool _autoIndex;
	size_t _maxBodySize;
	std::string _root;
	std::pair<int, std::string> _redirection;
	std::map<int, std::string> _errorPages;
	std::vector<std::string> _index;
	std::string _cgiPass;
	std::vector<LimitExceptDirective> _limitExcepts;

	const std::unordered_map<int, std::string> defaultErrorMsgs =
		{
			{400, "Bad Request"},
			{403, "Forbidden"},
			{404, "File Not Found"},
			{405, "Method Not Allowed"},
            {406, "Not Acceptable"},
			{408, "Request Timeout"},
			{413, "Content Too Large"},
			{415, "Unsupported Media Type"},
			{500, "Internal Server Error"},
			{504, "Gateway Timeout"}};

	const std::unordered_map<int, std::string> redirectionMsgs =
		{
			{301, "Moved Permanently"},
			{302, "Found"},
			{303, "See Other"},
			{307, "Temporary Redirect"},
			{308, "Permanent Redirect"}};

	const std::unordered_map<std::string, std::string> mimeTypes =
		{
			{".html", "text/html"},
			{".htm", "text/html"},
			{".txt", "text/plain"},
			{".jpg", "image/jpeg"},
			{".jpeg", "image/jpeg"},
			{".png", "image/png"},
	};

	Response(const Response &response) = delete;
	const Response &operator=(const Response &response) = delete;
};

#endif
