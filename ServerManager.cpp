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

void	ServerManager::checkTimeouts()
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(now - lastTimeoutCheck) > std::chrono::seconds(SERVER_TIMEOUT_CHECK_INTERVAL))
	{
		for (size_t i = 0; i < servers.size(); i++)
			servers[i]->checkForTimeouts();
		lastTimeoutCheck = now;
	}
}

void	ServerManager::processReadEvent(const struct kevent &event)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		if (event.ident == servers[i]->_socket || servers[i]->_clients.count(event.ident) > 0)
		{
			if (event.ident == servers[i]->_socket)
				servers[i]->acceptNewConnection();
			else
			{
				if (event.flags & EV_EOF)
				{
					std::cout << "Client disconnected" << std::endl;
					servers[i]->handleClientDisconnection(event.ident);
				}
				else
					servers[i]->handleClientRequest(event.ident);
			}
		}
	}
}

void	ServerManager::processWriteEvent(const struct kevent &event)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		if (servers[i]->_responses.count(event.ident) > 0)
		{
			servers[i]->handleClientResponse(event.ident);
			break;
		}
	}
}

void	ServerManager::start()
{
	this->lastTimeoutCheck = std::chrono::steady_clock::now();

	while (running)
	{
		checkTimeouts();

		std::cout << "Waiting for events" << std::endl;

		int nev = kqueue.waitForEvents();
		if (!running)
			break;


		if (nev < 0)
		{
			std::cerr << "Error in kqueue: " << strerror(errno) << std::endl;
			continue;
		}
		else if (nev == 0)
		{
			std::cout << "No events" << std::endl;
			continue;
		}
		

		for (int ev = 0; ev < nev; ev++)
		{
			if (kqueue.events[ev].filter == EVFILT_READ)
				processReadEvent(kqueue.events[ev]);
		}

		for (int ev = 0; ev < nev; ev++)
		{
			if (kqueue.events[ev].filter == EVFILT_WRITE)
				processWriteEvent(kqueue.events[ev]);
		}
	}

	std::cout << "ServerManager stopped" << std::endl;
	stop();
	
}

void	ServerManager::stop()
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		delete servers[i];
	}
}