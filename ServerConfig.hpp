


#pragma once
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include <sstream>
#include <string>



class ServerConfig
{

private:

	int			port;
	std::string	ipAddress;


public:
	void	setListen(const std::string &listenValue);
	bool	isValidPort(const std::string &port);
	bool	isValidIPv4();


};







#endif /* SERVERCONFIG_HPP */