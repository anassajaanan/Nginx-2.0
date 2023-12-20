#include "ServerConfig.hpp"


bool	ServerConfig::isValidIPv4()
{
	std::stringstream	ss(this->ipAddress);
	std::string			octet;
	int					num;
	int					count = 0;
	if (this->ipAddress.empty() || (this->ipAddress).back() == '.')
		return (false);
	while (std::getline(ss, octet, '.'))
	{
		if (ss.fail() || ss.bad())
			return (false);
		if (octet.empty() || octet.size() > 3)
			return (false);
		for (size_t i = 0; i < octet.size(); i++)
		{
			if (!std::isdigit(octet[i]))
				return (false);
		}
		num = std::stoi(octet);
		if (num < 0 || num > 255)
			return (false);
		count++;
	}
	return (count == 4);
}

bool	ServerConfig::isValidPort(const std::string &port)
{
	if (port.empty() || port.size() > 5)
		return (false);
	for (size_t i = 0; i < port.size(); i++)
	{
		if (!std::isdigit(port[i]))
			return (false);
	}
	this->port = std::stoi(port);
	return (this->port >= 0 && this->port <= 65535);
}

void	ServerConfig::setListen(const std::string &listenValue)
{
	std::string port = listenValue;
	size_t	colonPos = listenValue.find(':');
	if (colonPos != std::string::npos)
	{
		this->ipAddress = listenValue.substr(0, colonPos);
		port = listenValue.substr(colonPos + 1);
		if (!isValidIPv4())
			throw std::runtime_error("invalid IPv4 address in \"listen\" directive: \"" + this->ipAddress + "\"");
	}
	if (!isValidPort(port))
		throw std::runtime_error("invalid port in \"" + listenValue + "\" of the \"listen\" directive");
}