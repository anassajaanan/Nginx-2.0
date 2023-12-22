


#pragma once
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "TryFilesDirective.hpp"
#include "ReturnDirective.hpp"
#include "RewriteDirective.hpp"

#include <sstream>



class ServerConfig
{

private:
	bool					isValidPort(const std::string &port);
	bool					isValidIPv4();
	void					processFallbackStatusCode(const std::string &statusCode);
	bool					isValidAutoindex(const std::string &autoindexValue);
	void					splitValueAndUnit(const std::string &bodySize, std::string &value, std::string &unit);
	size_t					safeStringToSizeT(const std::string &bodySize, const std::string value);

public:

	int							port;
	std::string					ipAddress;
	std::string					root;
	std::string					index;
	std::string					serverName;
	std::string					autoindex;
	TryFilesDirective			tryFiles;
	ReturnDirective				returnDirective;
	RewriteDirective			rewriteDirective;
	size_t						clientMaxBodySize;
	std::map<int, std::string>	errorPages;


	// Constructors for default values


	// set Root Directive
	void					setRoot(const std::string &rootValue);
	
	// set Index directive
	void					setIndex(const std::string &indexValue);

	// set Listen directive
	void					setListen(const std::string &listenValue);
	
	// set ServerName directive
	void					setServerName(const std::string &serverNameValue);

	// set TryFiles directive
	void					setTryFiles(const std::vector<std::string> &tryFilesValue);

	// set Return directive
	void					setReturn(const std::vector<std::string> &returnValue);

	// set Rewrite directive
	void					setRewrite(const std::vector<std::string> &rewriteValue);

	// set Autoindex directive
	void					setAutoindex(const std::string &autoindexValue);
	

	// set ClientMaxBodySize directive
	void					setClientMaxBodySize(const std::string &bodySize);

	// set ErrorPage directive
	void					setErrorPage(const std::string &errorCode, const std::string &errorPageValue);
};

#endif /* SERVERCONFIG_HPP */