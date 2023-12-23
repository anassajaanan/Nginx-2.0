

// #include "ConfigParser.hpp"
#include "ConfigNode.hpp"
#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "ConfigParser.hpp"


void	initializeServers(ConfigNode *treeRoot, std::vector<ServerConfig> &servers)
{
	std::string root = "/var/www/html";
	std::string index = "index.html";
	std::string autoindex = "off";
	std::string client_max_body_size = "1m";
	std::vector<DirectiveNode *> errorPagesDirectives;


	if (treeRoot->getType() != Context || ((ContextNode *)treeRoot)->getName() != "http")
		throw std::runtime_error("Invalid config file: http context not found");
	

	const std::vector<ConfigNode *> &children = ((ContextNode *)treeRoot)->getChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i]->getType() == Directive)
		{
			DirectiveNode *directive = static_cast<DirectiveNode *>(children[i]);
			if (directive->getKey() == "root")
				root = directive->getValues()[0];
			else if (directive->getKey() == "index")
				index = directive->getValues()[0];
			else if (directive->getKey() == "autoindex")
				autoindex = directive->getValues()[0];
			else if (directive->getKey() == "client_max_body_size")
				client_max_body_size = directive->getValues()[0];
			else if (directive->getKey() == "error_page")
				errorPagesDirectives.push_back(directive);
		}
		else {
			ContextNode *context = static_cast<ContextNode *>(children[i]);
			if (context->getName() == "server")
			{
				servers.push_back(ServerConfig(root, index, autoindex, client_max_body_size, errorPagesDirectives));
				ServerConfig &server = servers.back();
				const std::vector<ConfigNode *> &serverChildren = context->getChildren();
				for (size_t j = 0; j < serverChildren.size(); j++)
				{
					if (serverChildren[i]->getType() == Directive)
					{
						DirectiveNode *serverDirective = static_cast<DirectiveNode *>(serverChildren[i]);
						if (serverDirective->getKey() == "listen")
							server.setListen(serverDirective->getValues()[0]);
						else if (serverDirective->getKey() == "server_name")
							server.setServerName(serverDirective->getValues()[0]);
						else if (serverDirective->getKey() == "client_max_body_size")
							server.setClientMaxBodySize(serverDirective->getValues()[0]);
						else if (serverDirective->getKey() == "error_page")
							server.setErrorPage(serverDirective->getKey(), serverDirective->getValues()[0]);
						else if (serverDirective->getKey() == "root")
							server.setRoot(serverDirective->getValues()[0]);
						else if (serverDirective->getKey() == "index")
							server.setIndex(serverDirective->getValues()[0]);
						else if (serverDirective->getKey() == "autoindex")
							server.setAutoindex(serverDirective->getValues()[0]);
						else if (serverDirective->getKey() == "try_files")
							server.setTryFiles(serverDirective->getValues());
						else if (serverDirective->getKey() == "return")
							server.setReturn(serverDirective->getValues());
						else if (serverDirective->getKey() == "rewrite")
							server.setRewrite(serverDirective->getValues());
					}
					else
					{
						ContextNode *location = static_cast<ContextNode *>(serverChildren[i]);
						if (location->getName() == "location")
						{
							server.addLocation(location);
						}
					}
				}
			}
		}
	}
}



int main()
{
    try
	{
		ConfigParser parser("nginx.conf");

		ConfigNode *treeRoot = parser.getConfigTreeRoot();

		std::vector<ServerConfig> servers;
		initializeServers(treeRoot, servers);

		std::cout << "Successfully parsed config file" << std::endl;
		
		delete treeRoot;


		return (0);

	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;

	
}