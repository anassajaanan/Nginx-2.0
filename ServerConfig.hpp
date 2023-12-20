


#pragma once
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
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

	public:
		void				setListen(const std::string &listenValue);
		void				setRoot(const std::string &rootValue);
		bool				isValidPort(const std::string &port);
		bool				isValidAutoindex(const std::string &autoindexValue);
		bool				isValidIPv4();
		const std::string	&getRoot();
		const std::string	&getAutoindex();
		void				setAutoindex(const std::string &autoindexValue);
		void				setClientMaxBodySize(const std::string &bodySize);
		int			getClientMaxBodySize();
		bool				isValidBodySize(const std::string &bodySize);


};







#endif /* SERVERCONFIG_HPP */