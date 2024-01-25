


#pragma once
#ifndef CONFIGLOADER_HPP
# define CONFIGLOADER_HPP

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"

// define default values
#define DEFAULT_HTTP_ROOT_VALUE "/var/www/html"
#define DEFAULT_HTTP_INDEX_VALUE "index.html"
#define DEFAULT_HTTP_AUTOINDEX_VALUE "off"
#define DEFAULT_HTTP_KEEPALIVE_TIMEOUT "15s"
#define DEFAULT_HTTP_CLIENT_MAX_BODY_SIZE "1m"




class ConfigLoader
{
private:
	ConfigNode	*treeRootNode;

	void processHttpNode(ContextNode* httpNode, std::vector<ServerConfig> &servers);
    void processServerNode(ContextNode* serverNode, ServerConfig &serverConfig);
    void processLocationNode(ContextNode* locationNode, LocationConfig &locationConfig);

public:
	std::string						root;
	std::vector<std::string>		index;
	std::string						autoindex;
	std::string						keepalive_timeout;		
	std::string						client_max_body_size;
	std::vector<DirectiveNode *>	errorPagesDirectives;

	ConfigLoader(ConfigNode *treeRoot);

	void	loadServers(std::vector<ServerConfig> &servers);
};


#endif /* CONFIGLOADER_HPP */
