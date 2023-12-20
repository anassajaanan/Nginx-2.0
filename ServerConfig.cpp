#include "ServerConfig.hpp"
#include <sys/_types/_size_t.h>


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

void	ServerConfig::setServerName(const std::string &serverNameValue)
{
	this->serverName = serverNameValue;
}

void	ServerConfig::setTryFiles(const std::vector<std::string> &tryFilesValue)
{
	size_t i;
	for (i = 0; i < tryFilesValue.size() - 1; i++)
		this->tryFiles.addPath(tryFilesValue[i]);
	if (tryFilesValue[i][0] == '=')
		processFallbackStatusCode(tryFilesValue[i]);
	else
		this->tryFiles.setFallBackUri(tryFilesValue[i]);
}

void	ServerConfig::processFallbackStatusCode(const std::string &statusCode)
{
	std::string code = statusCode.substr(1);
	if (code.empty() || code.size() > 3)
		throw std::runtime_error("invalid code in \"try_files\" directive: \"" + statusCode + "\"");
	for (size_t j = 0; j < code.size(); j++)
	{
		if (!std::isdigit(code[j]))
			throw std::runtime_error("invalid code in \"try_files\" directive: \"" + statusCode + "\"");
	}
	int codeInt = std::stoi(code);
	if (codeInt < 300 || codeInt > 599)
		throw std::runtime_error("invalid code in \"try_files\" directive: \"" + statusCode + "\"" + " (must be between 300 and 599)");
	this->tryFiles.setFallBackStatusCode(codeInt);
}