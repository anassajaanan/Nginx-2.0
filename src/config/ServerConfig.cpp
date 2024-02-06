#include "ServerConfig.hpp"
#include "LocationConfig.hpp"


ServerConfig::ServerConfig() { }

ServerConfig::ServerConfig(const std::string &rootValue, const std::vector<std::string> &indexValues,
				const std::string &autoindexValue, const std::string &keepaliveValue, const std::string &client_max_body_size,
				const std::vector<DirectiveNode *> &errorPagesDirectives)
{
	setDefaultValues();
	setRoot(rootValue);
	setIndex(indexValues);
	setAutoindex(autoindexValue);
	setKeepaliveTimeout(keepaliveValue);
	setClientMaxBodySize(client_max_body_size);
	for (size_t i = 0; i < errorPagesDirectives.size(); i++)
		setErrorPage(errorPagesDirectives[i]->getValues(), "Http");
}

void	ServerConfig::setDefaultValues()
{
	this->port = DEFAULT_SERVER_PORT;
	this->ipAddress = DEFAULT_SERVER_IP;
	this->root = DEFAULT_SERVER_ROOT;
	this->index.push_back(DEFAULT_SERVER_INDEX);
	this->serverName = DEFAULT_SERVER_NAME;
	this->autoindex = DEFAULT_SERVER_AUTOINDEX;
	this->clientMaxBodySize = DEFAULT_CLIENT_MAX_BODY_SIZE;
}


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

bool	ServerConfig::isValidPort(const std::string &portValue)
{
	if (portValue.empty() || portValue.size() > 5)
		return (false);
	for (size_t i = 0; i < portValue.size(); i++)
	{
		if (!std::isdigit(portValue[i]))
			return (false);
	}
	this->port = std::stoi(portValue);
	return (this->port >= 0 && this->port <= 65535);
}

void	ServerConfig::setListen(const std::string &listenValue)
{
	std::string portString = listenValue;
	size_t	colonPos = listenValue.find(':');
	if (colonPos != std::string::npos)
	{
		this->ipAddress = listenValue.substr(0, colonPos);
		portString = listenValue.substr(colonPos + 1);
		if (!isValidIPv4())
			throw std::runtime_error("invalid IPv4 address in \"listen\" directive: \"" + this->ipAddress + "\"");
	}
	if (!isValidPort(portString))
		throw std::runtime_error("invalid port in \"" + listenValue + "\" of the \"listen\" directive");
}

void	ServerConfig::setServerName(const std::string &serverNameValue)
{
	this->serverName = serverNameValue;
}

void	ServerConfig::setKeepaliveTimeout(const std::string &keepaliveTimeoutValue)
{
	std::string	timeout = keepaliveTimeoutValue;
	std::string unit;

	size_t pos = keepaliveTimeoutValue.find_first_not_of("0123456789");
	if (pos != std::string::npos)
	{
		if (pos == 0)
			throw std::runtime_error("invalid value in \"keepalive_timeout\" directive: \"" + keepaliveTimeoutValue + "\"");
		timeout = keepaliveTimeoutValue.substr(0, pos);
		unit = keepaliveTimeoutValue.substr(pos);
	}
	if (!unit.empty() && unit != "s")
		throw std::runtime_error("invalid unit in \"keepalive_timeout\" directive. It must be \"s\"");
	this->keepalive_timeout = std::stoi(timeout);
	if (this->keepalive_timeout < MIN_KEEPALIVE_TIMEOUT || this->keepalive_timeout > MAX_KEEPALIVE_TIMEOUT)
		throw std::runtime_error("invalid value in \"keepalive_timeout\" directive: \"" + keepaliveTimeoutValue + "\", it must be between 5s and 300s");
}

void	ServerConfig::addLocation(const std::string &path, const LocationConfig &locationConfig)
{
	if (locations.find(path) != locations.end())
		throw std::runtime_error("duplicate location \"" + path + "\" in Config file");
	locations[path] = locationConfig;
}
void	ServerConfig::setCgiExtension(const std::vector<std::string> &extensionsValue)
{
	if (extensionsValue.empty())
			return;
		for (size_t i = 0; i < extensionsValue.size();i++)
		{
			if (extensionsValue[i].find('.') == std::string::npos || extensionsValue[i].size() <= 2 || extensionsValue[i].substr(0, 2) != "*.") // optional give a max length 
				throw (std::runtime_error("invalid cgi extension in \"cgi_extension\" directive"));
		}
		// this->_isEnabled = true;
		for (size_t i = 0; i < extensionsValue.size(); i++)
			cgiExtension.addCgiExtension(extensionsValue[i].substr(1, extensionsValue[i].length()));
}

std::map<std::string, LocationConfig>	&ServerConfig::getLocations()
{
	return (this->locations);
}

LocationConfig	*ServerConfig::matchLocation(const std::string &uri)
{
	LocationConfig	*location = NULL;
	std::string		path;

	std::map<std::string, LocationConfig>::iterator it = locations.begin();
	
	for (; it != locations.end(); it++)
	{
		std::string locationPath = it->first;
		if (locationPath[0] == '=') // exact matching
		{
			if (uri == locationPath.substr(1))
				return &(it->second);
		}
		else if (uri.substr(0, locationPath.length()) == locationPath) // prefix matching
		{
			if (location == NULL || locationPath.length() > path.length())
			{
				location = &(it->second);
				path = locationPath;
			}
		}
	}
	return (location);
}
