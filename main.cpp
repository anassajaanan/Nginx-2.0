

// #include "ConfigParser.hpp"
#include "ConfigNode.hpp"
#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"
#include "Server.hpp"
#include <stdexcept>
#include <sys/event.h>


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

		// std::cout << "Successfully parsed config file" << std::endl;


		int kq = kqueue();
		if (kq == -1)
		{
			throw std::runtime_error("kqueue() failed");
		}


	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;

	
}