

// #include "ConfigParser.hpp"
#include "ConfigNode.hpp"
#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"
#include "Server.hpp"
#include "KqueueManager.hpp"

int main()
{
    try
	{
		ConfigParser parser("nginx.conf");
		parser.parseConfigFile();

		ConfigNode *treeRoot = parser.getConfigTreeRoot();

		std::vector<ServerConfig>	serverConfigs;
		std::vector<Server *> 		servers;
		
		ConfigLoader loader(treeRoot);
		loader.loadServers(serverConfigs);

		// std::cout << "Successfully parsed config file" << std::endl;


		KqueueManager	kqueue;

		for (size_t i = 0; i < serverConfigs.size(); i++)
		{

			Server *server = new Server(serverConfigs[i], kqueue);
			server->run();
			servers.push_back(server);
		}

		std::cout << "Successfully started server" << std::endl;
		std::cout << "Listening on port " << serverConfigs[0].port << std::endl;

		while (true)
		{
			int nev = kqueue.waitForEvents();
			std::cout << "nev: " << nev << std::endl; // TODO: remove this line
			if (nev < 0)
				throw std::runtime_error("Error in kqueue");
			for (int ev = 0; ev < nev; ev++)
			{
				for (size_t i = 0; i < servers.size(); i++)
				{
					if (kqueue.events[ev].ident == servers[i]->_socket || servers[i]->_clients.count(kqueue.events[ev].ident) > 0)
					{
						if (kqueue.events[ev].ident == servers[i]->_socket)
							servers[i]->acceptNewConnection();
						else
						{
							if (kqueue.events[ev].flags & EV_EOF)
								servers[i]->handleClientDisconnection(kqueue.events[ev].ident);
							else
								servers[i]->handleClientRequest(kqueue.events[ev].ident);
						}
						break;
					}
				}
			}
			
		}

		for (size_t i = 0; i < servers.size(); i++)
		{
			delete servers[i];
		}
		
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;

	
}