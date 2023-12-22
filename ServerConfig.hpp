


#pragma once
#include <sys/_types/_size_t.h>
#include <vector>
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "TryFilesDirective.hpp"
#include "ReturnDirective.hpp"
#include "RewriteDirective.hpp"

#include <sstream>
#include <string>

#define CLIENT_MAXIMUM_BODY_SIZE 1000000 /* MB */


class ServerConfig
{

private:

	int			maxBodySize;
	std::string	autoindex;
	std::string		index;
	std::vector<std::string>		errorPage;


	int					port;
	std::string			ipAddress;
	std::string			root;
	std::string			serverName;
	TryFilesDirective	tryFiles;
	ReturnDirective		returnDirective;
	RewriteDirective	rewriteDirective;
	size_t				clientMaxBodySize;

public:
	ServerConfig();


	// Root Directive
	void					setRoot(const std::string &rootValue);
	
	// set Index directive
	void					setIndex(const std::string &indexValue);

	// Listen directive
	void					setListen(const std::string &listenValue);
	bool					isValidPort(const std::string &port);
	bool					isValidIPv4();
	// ServerName directive
	void					setServerName(const std::string &serverNameValue);

	// set TryFiles directive
	void					setTryFiles(const std::vector<std::string> &tryFilesValue);
	void					processFallbackStatusCode(const std::string &statusCode);

	// set Return directive
	void					setReturn(const std::vector<std::string> &returnValue);

	// set Rewrite directive
	void					setRewrite(const std::vector<std::string> &rewriteValue);

	// set Autoindex directive
	void					setAutoindex(const std::string &autoindexValue);
	bool					isValidAutoindex(const std::string &autoindexValue);

	// set ClientMaxBodySize directive
	void					setClientMaxBodySize(const std::string &bodySize);
	void					splitValueAndUnit(const std::string &bodySize, std::string &value, std::string &unit);
	size_t					safeStringToSizeT(const std::string &bodySize, const std::string value);

	


	
	


	const std::string		&getRoot() const;

	
	
	
	
	const std::string		&getIndex() const;

	int						getClientMaxBodySize()const;
	


	void					setErrorPage(const std::string &errorCode, const std::string &directory);
	int						getErrorPageCode() const;
	const std::string		&getErrorPageDirectory() const;

	const std::string		&getAutoindex() const;
};

#endif /* SERVERCONFIG_HPP */