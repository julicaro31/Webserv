#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "Server.hpp"
#include "ParsingHelper.hpp"
#include <set>
#include <filesystem>
#include <unordered_map>
#include <string>

class Response
{
public:
	Response(const std::string &uri, const Server &server);

	// Once the Request is merged, this method can have a Request object as paremeter
	void setStatusAndMsg(Method method, const std::string &uri, const std::string &clientHost, const std::string &body);
	int getStatus() const;
	std::string getMsg() const;

private:
	int _status;
	std::string _msg;

	bool isCGI() const;
	bool isFile(const std::string &uri) const;
	const std::string getFullPath(const std::string &path);
	const std::string getFileContent(const std::string &fullPath);

	// Error msgs
	void handleResponseError(int status);
	std::string getAllowedMethods() const;
	bool isAllowed(Method method, const std::string &clientHost) const;

	// Redirection
	void handleRedirection();

	// Get request
	void handleGetRequest(const std::string &uri, const std::string &clientHost);
	void handleFileServing(const std::string &path);
	std::string getMimeType(const std::string &fileName);
	void handleAutoIndex(const std::string &path);

	// Post request
	void handlePostRequest(const std::string &uri, const std::string &clientHost, const std::string &body);
	void handleUpload(const std::string &uri, const std::string &body);

	// Delete request
	void handleDeleteRequest(const std::string &uri, const std::string &clientHost);
	void handleDeletion(const std::string &path);

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
			{413, "Content Too Large"},
			{500, "Internal Server Error"}};

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
};

void testResponse(const std::string &uri, const Server &server);

#endif