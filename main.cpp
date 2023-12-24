

// #include "ConfigParser.hpp"
#include "ConfigNode.hpp"
#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"



int main()
{
    try
	{
		ConfigParser parser("nginx.conf");
		parser.parseConfigFile();

		ConfigNode *treeRoot = parser.getConfigTreeRoot();

		std::vector<ServerConfig> servers;
		
		ConfigLoader loader(treeRoot);
		loader.loadServers(servers);

		std::cout << "Successfully parsed config file" << std::endl;

	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;

	
}