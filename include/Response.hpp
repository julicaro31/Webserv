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
	std::string getRequestMsg(Method method, const std::string &uri, const std::string clientHost);

private:
	// REQUEST HANDLERS
	void handleGetRequest(const std::string &uri, const std::string clientHost);
	void handlePostRequest(const std::string &uri, const std::string clientHost);
	void handleDeleteRequest(const std::string &uri, const std::string clientHost);
	void handleBadRequest();

	// METHOD NOT ALLOW
	void handleMethodNotAllowed();
	std::string getAllowedMethods() const;
	bool isAllowed(Method method, const std::string clientHost) const;

	// SERVE FILE
	void handleFileServing(const std::string &path);
	const std::string readFile(const std::string &path);
	std::string getMimeType(const std::string& fileName);
	void handleAutoIndex(const std::string &path);

	// FILE NOT FOUND
	void handleFileNotFound();

	bool isCGI() const;
	bool isFile(const std::string &uri) const;

	bool _autoIndex;
	size_t _maxBodySize;
	std::string _root;
	std::pair<int, std::string> _redirection;
	std::map<int, std::string> _errorPages;
	std::vector<std::string> _index;
	std::string _cgiPass;
	std::vector<LimitExceptDirective> _limitExcepts;

	std::string _msg;

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