#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

ServerConfig::ServerConfig() { }

ServerConfig::ServerConfig(const std::string &rootValue, const std::string &indexValue,
				const std::string &autoindexValue, const std::string &client_max_body_size,
				const std::vector<DirectiveNode *> &errorPagesDirectives)
{
	setDefaultValues();
	setRoot(rootValue);
	setIndex(indexValue);
	setAutoindex(autoindexValue);
	setClientMaxBodySize(client_max_body_size);
	for (size_t i = 0; i < errorPagesDirectives.size(); i++)
	{
		const std::vector<std::string> &values = errorPagesDirectives[i]->getValues();
		setErrorPage(errorPagesDirectives[i]->getKey(), values[0]);
	}
}

void	ServerConfig::setDefaultValues()
{
	this->port = DEFAULT_SERVER_PORT;
	this->ipAddress = DEFAULT_SERVER_IP;
	this->root = DEFAULT_SERVER_ROOT;
	this->index = DEFAULT_SERVER_INDEX;
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

void	ServerConfig::addLocation(ContextNode *locationNode)
{
	if (locations.find(locationNode->getPath()) != locations.end())
		throw std::runtime_error("duplicate location \"" + locationNode->getPath() + "\" in Config file");

	LocationConfig location(*this);
	const std::vector<ConfigNode *> &locationChildren = locationNode->getChildren();
	for (size_t i = 0; i < locationChildren.size(); i++)
	{
		if (locationChildren[i]->getType() == Directive)
		{
			DirectiveNode *directive = static_cast<DirectiveNode *>(locationChildren[i]);
			if (directive->getKey() == "root")
				location.setRoot(directive->getValues()[0]);
			else if (directive->getKey() == "index")
				location.setIndex(directive->getValues()[0]);
			else if (directive->getKey() == "autoindex")
				location.setAutoindex(directive->getValues()[0]);
			else if (directive->getKey() == "client_max_body_size")
				location.setClientMaxBodySize(directive->getValues()[0]);
			else if (directive->getKey() == "error_page")
				location.setErrorPage(directive->getKey(), directive->getValues()[0]);
			else if (directive->getKey() == "try_files")
				location.setTryFiles(directive->getValues());
			else if (directive->getKey() == "return")
				location.setReturn(directive->getValues());
			else if (directive->getKey() == "rewrite")
				location.setRewrite(directive->getValues());
		}
	}
	locations[locationNode->getPath()] = location;
}
