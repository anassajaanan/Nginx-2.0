#include "ServerManager.hpp"


int	ServerManager::running = 1;

ServerManager::ServerManager(std::vector<ServerConfig> &serverConfigs, MimeTypeParser &mimeTypes)
{
	initializeServers(serverConfigs, mimeTypes);
}

ServerManager::~ServerManager() { }

void	ServerManager::initializeServers(std::vector<ServerConfig> &serverConfigs, MimeTypeParser &mimeTypes)
{
	for (size_t i = 0; i < serverConfigs.size(); i++)
	{
		Server *server = new Server(serverConfigs[i], mimeTypes, kqueue);
		server->run();
		servers.push_back(server);
	}
}

void	ServerManager::start()
{
	this->lastTimeoutCheck = std::chrono::steady_clock::now();

	while (running)
	{
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - lastTimeoutCheck) > std::chrono::seconds(SERVER_TIMEOUT_CHECK_INTERVAL))
		{
			for (size_t i = 0; i < servers.size(); i++)
				servers[i]->checkForTimeouts();
			lastTimeoutCheck = now;
		}


		std::cout << "Waiting for events" << std::endl;
		int nev = kqueue.waitForEvents();
		if (!running)
			break;
		std::cout << "nev: " << nev << std::endl;
		if (nev < 0)
			throw std::runtime_error("Error in kqueue");
		std::cout << "Events received => ";
		for (int ev = 0; ev < nev; ev++)
		{
			if (kqueue.events[ev].filter == EVFILT_READ)
				std::cout << "EVFILT_READ " << std::endl;
			else if (kqueue.events[ev].filter == EVFILT_WRITE)
				std::cout << "EVFILT_WRITE " << std::endl;
			else if (kqueue.events[ev].filter == EVFILT_EXCEPT)
				std::cout << "EVFILT_EXCEPT " << std::endl;
			else
				std::cout << "Unknown filter " << kqueue.events[ev].filter << std::endl;
		}
		std::cout << std::endl;
		for (int ev = 0; ev < nev; ev++)
		{
			if (kqueue.events[ev].filter == EVFILT_READ)
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
							{
								std::cout << "Client disconnected" << std::endl;
								servers[i]->handleClientDisconnection(kqueue.events[ev].ident);
							}
							else
								servers[i]->handleClientRequest(kqueue.events[ev].ident);
						}
						break;
					}
				}
			}
		}
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