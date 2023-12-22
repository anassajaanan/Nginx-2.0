#include "ServerConfig.hpp"
#include "ReturnDirective.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/_types/_size_t.h>
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

void	ServerConfig::setRewrite(const std::vector<std::string> &rewriteValue)
{
	this->rewriteDirective.setPattern(rewriteValue[0]);
	this->rewriteDirective.setSubstitution(rewriteValue[1]);
}

void		ServerConfig::setIndex(const std::string &indexValue)
{
	this->index = indexValue;
}

void	ServerConfig::setRoot(const std::string &rootValue)
{
	if (!rootValue.empty())
		this->root = rootValue;
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



void	ServerConfig::splitValueAndUnit(const std::string &bodySize, std::string &value, std::string &unit)
{
	size_t pos = bodySize.find_first_not_of("0123456789");
	if (pos != std::string::npos)
	{
		value = bodySize.substr(0, pos);
		unit = bodySize.substr(pos);
	}
	else
		value = bodySize;
}

size_t	ServerConfig::safeStringToSizeT(const std::string &bodySize, const std::string value)
{
	try
	{
		return (std::stoull(value));
		
	}
	catch (const std::exception &e)
	{
		throw (std::runtime_error("invalid value \"" + bodySize + "\" in \"client_max_body_size\" directive"));
	}
}


void	ServerConfig::setClientMaxBodySize(const std::string &bodySize)
{
	size_t multiplier = 1; // in case there is no unit (bytes by default)
	size_t numericValue, totalSize;
	std::string value, unit;

	splitValueAndUnit(bodySize, value, unit);

	if (value.empty() || unit.size() > 1)
		throw (std::runtime_error("invalid value \"" + bodySize + "\" in \"client_max_body_size\" directive"));
	if (unit.size() == 1 && unit.find_first_not_of("kKmMgG") != std::string::npos)
		throw (std::runtime_error("invalid value \"" + bodySize + "\" in \"client_max_body_size\" directive"));
	if (unit == "k" || unit == "K")
		multiplier = 1000;
	else if (unit == "m" || unit == "M")
		multiplier = 1000000;
	else if (unit == "g" || unit == "G")
		multiplier = 1000000000;
	
	numericValue = safeStringToSizeT(bodySize, value);
	totalSize = numericValue * multiplier;
	// check for overflow
	if (totalSize / multiplier != numericValue)
		throw (std::runtime_error("invalid value \"" + bodySize + "\" in \"client_max_body_size\" directive"));
}







void	ServerConfig::setErrorPage(const std::string &errorCode, const std::string &errorPageValue)
{
	if (errorCode.empty() || errorCode.size() > 3)
		throw std::runtime_error("invalid code in \"error_page\" directive: \"" + errorCode + "\"");
	for (size_t j = 0; j < errorCode.size(); j++)
	{
		if (!std::isdigit(errorCode[j]))
			throw std::runtime_error("invalid code in \"error_page\" directive: \"" + errorCode + "\"");
	}
	int codeInt = std::stoi(errorCode);
	if (codeInt < 300 || codeInt > 599)
		throw std::runtime_error("invalid code in \"error_page\" directive: \"" + errorCode + "\"" + " (must be between 300 and 599)");
	errorPages[codeInt] = errorPageValue;
}
