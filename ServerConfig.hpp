


#pragma once
#include <vector>
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "TryFilesDirective.hpp"
#include "ReturnDirective.hpp"

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

public:
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


	void					setRoot(const std::string &rootValue);
	bool					isValidAutoindex(const std::string &autoindexValue);
	const std::string		&getRoot() const;
	const std::string		&getAutoindex() const;
	void					setAutoindex(const std::string &autoindexValue);
	void					setClientMaxBodySize(const std::string &bodySize);
	int						getClientMaxBodySize()const;
	bool					isValidBodySize(const std::string &bodySize);
	
	void					setIndex(const std::string &indexValue);
	const std::string		&getIndex() const;


	


	void					setErrorPage(const std::string &errorCode, const std::string &directory);
	int						getErrorPageCode() const;
	const std::string		&getErrorPageDirectory() const;
};

#endif /* SERVERCONFIG_HPP */