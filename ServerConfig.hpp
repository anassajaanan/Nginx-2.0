


#pragma once
#include "TreeBuilder.hpp"
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "TryFilesConfig.hpp"

#include <sstream>
#include <string>
#define CLIENT_MAXIMUM_BODY_SIZE 1000000 /*MB*/


class ServerConfig
{

	private:

		int			port;
		int			maxBodySize;
		std::string	ipAddress;
		std::string	root;
		std::string	autoindex;
		std::string		serverName;
		TryFilesConfig	tryFiles;
		std::string		index;
		std::vector<std::string>		errorPage;

	public:
		void					setListen(const std::string &listenValue);
		void					setRoot(const std::string &rootValue);
		bool					isValidPort(const std::string &port);
		bool					isValidAutoindex(const std::string &autoindexValue);
		bool					isValidIPv4();
		const std::string		&getRoot() const;
		const std::string		&getAutoindex() const;
		void					setAutoindex(const std::string &autoindexValue);
		void					setClientMaxBodySize(const std::string &bodySize);
		int						getClientMaxBodySize()const;
		bool					isValidBodySize(const std::string &bodySize);
		void					setServerName(const std::string &serverNameValue);
		void					setIndex(const std::string &indexValue);
		const std::string		&getIndex() const;
		// set TryFiles directive
		void					setTryFiles(const std::vector<std::string> &tryFilesValue);
		void					setErrorPage(const std::string &errorCode, const std::string &directory);
		int						getErrorPageCode() const;
		const std::string		&getErrorPageDirectory() const;
		void					processFallbackStatusCode(const std::string &statusCode);
};

#endif /* SERVERCONFIG_HPP */