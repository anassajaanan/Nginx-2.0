

#pragma once
#include "ServerConfig.hpp"


class LocationConfig
{
private:
	void					processFallbackStatusCode(const std::string &statusCode);
	void					splitValueAndUnit(const std::string &bodySize, std::string &value, std::string &unit);
	size_t					safeStringToSizeT(const std::string &bodySize, const std::string value);

public:

	std::string					root;
	std::string					index;
	std::string					autoindex;
	std::map<int, std::string>	errorPages;
	size_t						clientMaxBodySize;

	TryFilesDirective			tryFiles;
	ReturnDirective				returnDirective;
	RewriteDirective			rewriteDirective;

	LocationConfig();
	LocationConfig(const ServerConfig &serverConfig);

	// set Root Directive
	void					setRoot(const std::string &rootValue);

	// set Index directive
	void					setIndex(const std::string &indexValue);

	// set Autoindex directive
	void					setAutoindex(const std::string &autoindexValue);

	// set ErrorPage directive
	void					setErrorPage(const std::string &statusCode, const std::string &uri);

	// set ClientMaxBodySize directive
	void					setClientMaxBodySize(const std::string &bodySize);

	// set TryFiles directive
	void					setTryFiles(const std::vector<std::string> &tryFilesValue);

	// set Return directive
	void					setReturn(const std::vector<std::string> &returnValue);

	// set Rewrite directive
	void					setRewrite(const std::vector<std::string> &rewriteValue);
};