#include "LocationConfig.hpp"
#include "ServerConfig.hpp"
#include <algorithm>
#include <stdexcept>

LocationConfig::LocationConfig() { }

LocationConfig::LocationConfig(const std::string &path, const ServerConfig &serverConfig)
{
	this->path = path;
	this->root = serverConfig.root;
	this->index = serverConfig.index;
	this->autoindex = serverConfig.autoindex;
	this->errorPages = serverConfig.errorPages;
	this->errorPagesContext = serverConfig.errorPagesContext;
	this->clientMaxBodySize = serverConfig.clientMaxBodySize;
}

void	LocationConfig::setAllowedMethods(const std::vector<std::string> &limitExceptValues)
{
	std::set<std::string>	acceptedMethods;

	acceptedMethods.insert("get");
	acceptedMethods.insert("put");
	acceptedMethods.insert("post");
	acceptedMethods.insert("head");
	acceptedMethods.insert("delete");

	std::string method;

	for (size_t i = 0; i < limitExceptValues.size(); i++)
	{
		method.resize(limitExceptValues[i].size());
		std::transform(limitExceptValues[i].begin(), limitExceptValues[i].end(), method.begin(), ::tolower);
		if (acceptedMethods.find(method) != acceptedMethods.end())
			this->allowedMethods.insert(method);
		else
		 	throw std::runtime_error("invalid method \"" + limitExceptValues[i] + "\" in limit_except directive");
	}
}

const std::string	&LocationConfig::getPath() const
{
	return (this->path);
}


bool	LocationConfig::isMethodAllowed(const std::string &method) const
{
	std::string	currentMethod;

	currentMethod.resize(method.size());
	std::transform(method.begin(), method.end(), currentMethod.begin(), ::tolower);
	if (this->allowedMethods.empty() || this->allowedMethods.find(currentMethod) != this->allowedMethods.end())
		return (true);
	return (false);
}