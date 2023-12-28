

// #include "ConfigParser.hpp"
#include "ConfigNode.hpp"
#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"
#include "Server.hpp"
#include "KqueueManager.hpp"

#include <csignal>

int running = 1;

void	signalHandler(int signum)
{
	std::cout << "Signal " << signum << " received" << std::endl;
	if (signum == SIGINT || signum == SIGTERM)
		running = 0;
}

int main()
{
    try
	{
		signal(SIGINT, signalHandler);
		signal(SIGTERM, signalHandler);

		ConfigParser parser("nginx.conf");
		parser.parseConfigFile();

		ConfigNode *treeRoot = parser.getConfigTreeRoot();

		std::vector<ServerConfig>	serverConfigs;
		std::vector<Server *> 		servers;
		
		ConfigLoader loader(treeRoot);
		loader.loadServers(serverConfigs);

		

		std::cout << "Successfully parsed config file" << std::endl;


		
		
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;

	
}