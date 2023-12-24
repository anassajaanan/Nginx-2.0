

// #include "ConfigParser.hpp"
#include "ConfigNode.hpp"
#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"



int main()
{
	ConfigNode *treeRoot = NULL;

    try
	{
		ConfigParser parser(&treeRoot, "nginx.conf");

		// ConfigNode *treeRoot = parser.getConfigTreeRoot();

		std::vector<ServerConfig> servers;
		
		ConfigLoader loader(treeRoot);
		loader.loadServers(servers);

		


		std::cout << "Successfully parsed config file" << std::endl;
		
		// delete treeRoot;


		return (0);

	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	if (treeRoot)
	{
		std::cout << "Tree is not empty" << std::endl;
	}
	else
	{
		std::cout << "Tree is empty" << std::endl;
	}

	return 0;

	
}