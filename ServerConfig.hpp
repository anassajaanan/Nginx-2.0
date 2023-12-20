


#pragma once
#include "TreeBuilder.hpp"
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "TryFilesConfig.hpp"

#include <sstream>
#include <string>



class ServerConfig
{

private:

	int				port;
	std::string		ipAddress;
	std::string		serverName;
	TryFilesConfig	tryFiles;


public:
	// set Listen directive
	void	setListen(const std::string &listenValue);
	bool	isValidPort(const std::string &port);
	bool	isValidIPv4();

	// set ServerName directive
	void	setServerName(const std::string &serverNameValue);

	// set TryFiles directive
	void	setTryFiles(const std::vector<std::string> &tryFilesValue);
	void	processFallbackStatusCode(const std::string &statusCode);




};







#endif /* SERVERCONFIG_HPP */