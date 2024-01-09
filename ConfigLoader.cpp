#include "ConfigLoader.hpp"


ConfigLoader::ConfigLoader(ConfigNode *treeRoot)
{
	this->root = DEFAULT_HTTP_ROOT_VALUE;
	this->index.push_back(DEFAULT_HTTP_INDEX_VALUE);
	this->autoindex = DEFAULT_HTTP_AUTOINDEX_VALUE;
	this->client_max_body_size = DEFAULT_HTTP_CLIENT_MAX_BODY_SIZE;
	this->treeRoot = treeRoot;
}


void ConfigLoader::processLocationNode(ContextNode* locationNode, LocationConfig &locationConfig)
{
	const std::vector<ConfigNode *> &locationChildren = locationNode->getChildren();
	for (size_t i = 0; i < locationChildren.size(); i++)
	{
		if (locationChildren[i]->getType() == Directive)
		{
			DirectiveNode *directive = static_cast<DirectiveNode *>(locationChildren[i]);
			if (directive->getKey() == "root")
				locationConfig.setRoot(directive->getValues()[0]);
			else if (directive->getKey() == "index")
				locationConfig.setIndex(directive->getValues());
			else if (directive->getKey() == "autoindex")
				locationConfig.setAutoindex(directive->getValues()[0]);
			else if (directive->getKey() == "client_max_body_size")
				locationConfig.setClientMaxBodySize(directive->getValues()[0]);
			else if (directive->getKey() == "error_page")
				locationConfig.setErrorPage(directive->getValues(), "Location");
			else if (directive->getKey() == "try_files")
				locationConfig.setTryFiles(directive->getValues());
			else if (directive->getKey() == "return")
				locationConfig.setReturn(directive->getValues());
			else if (directive->getKey() == "limit_except")
				locationConfig.setAllowedMethods(directive->getValues());
		}
	}
}


void	ConfigLoader::processServerNode(ContextNode* serverNode, ServerConfig &serverConfig)
{
	const std::vector<ConfigNode *>	&serverChildren = serverNode->getChildren();
	for (size_t i = 0; i < serverChildren.size(); i++)
	{
		if (serverChildren[i]->getType() == Directive)
		{
			DirectiveNode *directive = static_cast<DirectiveNode *>(serverChildren[i]);
			if (directive->getKey() == "listen")
				serverConfig.setListen(directive->getValues()[0]);
			else if (directive->getKey() == "server_name")
				serverConfig.setServerName(directive->getValues()[0]);
			else if (directive->getKey() == "client_max_body_size")
				serverConfig.setClientMaxBodySize(directive->getValues()[0]);
			else if (directive->getKey() == "error_page")
				serverConfig.setErrorPage(directive->getValues(), "Server");
			else if (directive->getKey() == "root")
				serverConfig.setRoot(directive->getValues()[0]);
			else if (directive->getKey() == "index")
				serverConfig.setIndex(directive->getValues());
			else if (directive->getKey() == "autoindex")
				serverConfig.setAutoindex(directive->getValues()[0]);
			else if (directive->getKey() == "try_files")
				serverConfig.setTryFiles(directive->getValues());
			else if (directive->getKey() == "return")
				serverConfig.setReturn(directive->getValues());
			
		}
		else
		{
			ContextNode	*locationNode = static_cast<ContextNode *>(serverChildren[i]);
			if (locationNode->getName() == "location")
			{
				LocationConfig location(locationNode->getPath(), serverConfig);
				processLocationNode(locationNode, location);
				serverConfig.addLocation(locationNode->getPath(), location);
			}
		}
	}
}

void	ConfigLoader::processHttpNode(ContextNode *treeRoot, std::vector<ServerConfig> &servers)
{
	if (!treeRoot || treeRoot->getType() != Context || treeRoot->getName() != "http")
		throw std::runtime_error("Invalid config file: http context not found");

	const std::vector<ConfigNode *>	&httpChildren = treeRoot->getChildren();
	for (size_t i = 0; i < httpChildren.size(); i++)
	{
		if (httpChildren[i]->getType() == Directive)
		{
			DirectiveNode *directive = static_cast<DirectiveNode *>(httpChildren[i]);
			if (directive->getKey() == "root")
				this->root = directive->getValues()[0];
			else if (directive->getKey() == "index")
				this->index = directive->getValues();
			else if (directive->getKey() == "autoindex")
				this->autoindex = directive->getValues()[0];
			else if (directive->getKey() == "client_max_body_size")
				this->client_max_body_size = directive->getValues()[0];
			else if (directive->getKey() == "error_page")
				this->errorPagesDirectives.push_back(directive);
		}
		else
		{
			ContextNode *serverNode = static_cast<ContextNode *>(httpChildren[i]);
			if (serverNode->getName() == "server")
			{
				servers.push_back(ServerConfig(this->root, this->index, this->autoindex, this->client_max_body_size, this->errorPagesDirectives));
				ServerConfig &server = servers.back();
				processServerNode(serverNode, server);
			}
		}
	}
}

void	ConfigLoader::loadServers(std::vector<ServerConfig> &servers)
{
	ContextNode *httpNode = static_cast<ContextNode *>(treeRoot);
	processHttpNode(httpNode, servers);
}