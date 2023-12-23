#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

LocationConfig::LocationConfig() { }

LocationConfig::LocationConfig(const ServerConfig &serverConfig)
{
	this->root = serverConfig.root;
	this->index = serverConfig.index;
	this->autoindex = serverConfig.autoindex;
	this->errorPages = serverConfig.errorPages;
	this->clientMaxBodySize = serverConfig.clientMaxBodySize;
}
