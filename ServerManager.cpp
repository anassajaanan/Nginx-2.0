#include "ServerManager.hpp"
#include "Logger.hpp"
#include <string>


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
		if (server->_socket == -1)
		{
			Logger::log(Logger::ERROR, "Failed to create server", "ServerManager::initializeServers");
			delete server;
			continue;
		}
		Logger::log(Logger::INFO, "Server is created and it is listening on port: " + std::to_string(server->_config.port), "ServerManager::initializeServers");
		kqueue.registerEvent(server->_socket, EVFILT_READ);
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
					servers[i]->handleClientDisconnection(event.ident);
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

		Logger::log(Logger::DEBUG, "Waiting for events", "EventLoop");

		int nev = kqueue.waitForEvents();
		if (!running)
			break;


		if (nev < 0)
		{
			Logger::log(Logger::ERROR, "Error in kqueue: " + std::string(strerror(errno)), "EventLoop");
			continue;
		}
		else if (nev == 0)
		{
			Logger::log(Logger::DEBUG, "No events to process at this time", "EventLoop");
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