#include "ServerConfig.hpp"
#include "ReturnDirective.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


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


void	ServerConfig::setReturn(const std::vector<std::string> &returnValue)
{
	if (returnValue.size() == 2)
		this->returnDirective.setResponseTextOrUrl(returnValue[1]);
	std::string code = returnValue[0];
	if (code.empty() || code.size() > 3)
		throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"");
	for (size_t j = 0; j < code.size(); j++)
	{
		if (!std::isdigit(code[j]))
			throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"");
	}
	int codeInt = std::stoi(code);
	if (codeInt < 300 || codeInt > 599)
		throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"" + " (must be between 300 and 599)");
	this->returnDirective.setStatusCode(codeInt);

}











void	ServerConfig::setRoot(const std::string &rootValue)
{
	if (rootValue.empty())
		return ;
	this->root = rootValue;
}

const	std::string &ServerConfig::getRoot() const
{
	return  (this->root);
}

bool	ServerConfig::isValidAutoindex(const std::string &autoindexValue)
{
	if (autoindexValue.empty() || (autoindexValue != "on" && autoindexValue != "off"))
		throw (std::runtime_error("invalid value \"" + autoindexValue + "\" in \"autoindex\" directive, it must be \"on\" or \"off\""));
	return (true);
}

void	ServerConfig::setAutoindex(const std::string &autoindexValue)
{
	isValidAutoindex(autoindexValue);
	this->autoindex = autoindexValue;
}

const	std::string &ServerConfig::getAutoindex() const
{
	return  (this->autoindex);
}

bool	ServerConfig::isValidBodySize(const std::string &bodySize)
{
	std::string	num;
	std::string	tmp;
	std::stringstream	ss;

	for (size_t i = 0; i < bodySize.length(); i++)
	{
		if (!std::isdigit(bodySize[i]))
		{
			num = bodySize.substr(0, i);
			tmp = bodySize.substr(i, bodySize.length());
			break;
		}

	}
	if (num.empty() || tmp.find_first_not_of("KMGkmg") != std::string::npos || tmp.length() > 1)
		throw (std::runtime_error("\"client_max_body_size\" directive invalid value"));
	ss << num;
	ss >> this->maxBodySize;
	if (ss.fail())
		throw (std::runtime_error("\"client_max_body_size\" directive invalid value"));
	return (true);
}


void	ServerConfig::setErrorPage(const std::string &errorCode, const std::string &directory)
{
	this->errorPage.push_back(errorCode);
	this->errorPage.push_back(directory);
}

/*returns the error page code for error page*/
int		ServerConfig::getErrorPageCode() const
{
	int					errorCode;
	std::stringstream	ss;

	ss << this->errorPage[0];
	ss >> errorCode;
	return (errorCode);
}

/*returns the error page directory for error page*/
const std::string		&ServerConfig::getErrorPageDirectory() const
{
	return (this->errorPage[1]);
}

void		ServerConfig::setIndex(const std::string &indexValue)
{
	this->index = indexValue;
}

const std::string		&ServerConfig::getIndex() const
{
	return  (this->index);
}

void	ServerConfig::setClientMaxBodySize(const std::string &bodySize)
{
	isValidBodySize(bodySize);
}

int	ServerConfig::getClientMaxBodySize() const
{
	return (maxBodySize);
}

/* client_body_size NGINX has a limit of 1MB on file uploads*/
