#include "HttpParser.hpp"

void HttpParser::parseRequest(std::string strRequest)
{
	//	symbol table
	//	analysis
	//
	//	lexical analyzer
	//		token stream
	//			divide into lexemes
	//			map to symbol table
	//		pass to syntax analyzer
	//
	//	syntax analyzer
	//		create syntax-tree of token stream
	//
	//	semantic analyzer
	//		use syntax tree and symbol table
	//		type checking
	//
	//	synthesis / code generation
	//		map into request
	

	// std::stringstream streamStrRequest(strRequest);
	// std::string method, target, version;
	//
	// streamStrRequest >> method >> target >> version;
	//
	// std::map<std::string, std::string> headers;
	// std::string header;
	// std::getline(streamStrRequest, header);
	// while (getline(streamStrRequest, header))
	// {
	// 	if (header == "\r" || header.empty())
	// 	{
	// 		break;
	// 	}
	//
	// 	size_t colonPos = header.find(':');
	// 	if (colonPos != std::string::npos)
	// 	{
	// 		std::string headerName = header.substr(0, colonPos);
	// 		std::string headerValue = std::string("lol");//ParsingHelper::trim(header.substr(colonPos + 1));
	// 		headers[headerName] = headerValue;
	// 	}
	// }

	// Request request(method, target, version, headers);

	// return Request();
}

