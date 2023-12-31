

// #include "ConfigParser.hpp"
#include "ConfigNode.hpp"
#include "ServerConfig.hpp"
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"
#include "Server.hpp"
#include "KqueueManager.hpp"

#include "RequestHandler.hpp"

#include "MimeTypeParser.hpp"

#include <csignal>
#include <sys/event.h>

int running = 1;

void	signalHandler(int signum)
{
	std::cout << "Signal " << signum << " received" << std::endl;
	if (signum == SIGINT || signum == SIGTERM)
		running = 0;
}

void	start(std::vector<Server *> &servers, std::vector<ServerConfig> &serverConfigs, MimeTypeParser &mimeTypes)
{
	KqueueManager	kqueue;

	for (size_t i = 0; i < serverConfigs.size(); i++)
	{

		Server *server = new Server(serverConfigs[i], mimeTypes, kqueue);
		server->run();
		servers.push_back(server);
	}


	while (running)
	{
		std::cout << "Waiting for events" << std::endl;
		int nev = kqueue.waitForEvents();
		if (!running)
			break;
		std::cout << "nev: " << nev << std::endl;
		if (nev < 0)
			throw std::runtime_error("Error in kqueue");
		for (int ev = 0; ev < nev; ev++)
		{
			if (kqueue.events[ev].filter == EVFILT_WRITE)
			{
				for (size_t i = 0; i < servers.size(); i++)
				{
					if (servers[i]->_responses.count(kqueue.events[ev].ident) > 0)
					{
						servers[i]->handleClientResponse(kqueue.events[ev].ident);
						break;
					}
				}
			}
			else if (kqueue.events[ev].filter == EVFILT_READ)
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
			else if (kqueue.events[ev].filter == EVFILT_EXCEPT)
			{
				std::cout << "Exception" << std::endl;
			}
		}
		
	}

	for (size_t i = 0; i < servers.size(); i++)
	{
		servers[i]->stop();
		delete servers[i];
	}
}

int main()
{
    try
	{
		signal(SIGINT, signalHandler);
		signal(SIGTERM, signalHandler);

		ConfigParser parser("nginx.conf");
		parser.parseConfigFile();

		MimeTypeParser mimeTypeParser("mime.types");
		mimeTypeParser.parseMimeTypeFile();

		ConfigNode *treeRoot = parser.getConfigTreeRoot();

		std::vector<ServerConfig>	serverConfigs;
		std::vector<Server *> 		servers;
		
		ConfigLoader loader(treeRoot);
		loader.loadServers(serverConfigs);

		// std::cout << "Successfully parsed config file" << std::endl;
		start(servers, serverConfigs, mimeTypeParser);


		


		
		

		
		
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;

	
}