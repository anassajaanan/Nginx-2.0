#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

LocationConfig::LocationConfig() { }

LocationConfig::LocationConfig(const std::string &path, const ServerConfig &serverConfig)
{
	this->path = path;
	this->root = serverConfig.root;
	this->index = serverConfig.index;
	this->autoindex = serverConfig.autoindex;
	this->errorPages = serverConfig.errorPages;
	this->clientMaxBodySize = serverConfig.clientMaxBodySize;
}

const std::string	&LocationConfig::getPath() const
{
	return (this->path);
}