#include "ServerManager.hpp"


int	ServerManager::running = 1;

ServerManager::ServerManager(std::vector<ServerConfig> &_serverConfigs, EventPoller *_eventManager, MimeTypeConfig &_mimeTypes)
	: eventManager(_eventManager)
{
	initializeServers(_serverConfigs, _mimeTypes);
}

ServerManager::~ServerManager() { }

void	ServerManager::initializeServers(std::vector<ServerConfig> &serverConfigs, MimeTypeConfig &mimeTypes)
{
	for (size_t i = 0; i < serverConfigs.size(); i++)
	{
		Server *server = new Server(serverConfigs[i], eventManager, mimeTypes);
		server->run();
		if (server->_socket == -1)
		{
			Logger::log(Logger::ERROR, "Failed to create server", "ServerManager::initializeServers");
			delete server;
			continue;
		}
		Logger::log(Logger::INFO, "Server is created and it is listening on port: " + std::to_string(server->_config.port), "ServerManager::initializeServers");
		eventManager->registerEvent(server->_socket, READ);
		servers.push_back(server);
	}
	displayStartupDetails();
}

void	ServerManager::displayStartupDetails()
{
	std::cout << "\n🔌 Overview of Active Servers:\n│\n";
	for (size_t i = 0; i < servers.size(); i++)
	{
		if (i < servers.size() - 1)
			std::cout << "├── \033[0;36m🔵 Listening on Port \033[1;33m" << std::to_string(servers[i]->_config.port) << "\033[0;36m 🌐\033[0m\n│\n";
		else
			std::cout << "└── \033[0;32m🟢 Listening on Port \033[1;33m" << std::to_string(servers[i]->_config.port) << "\033[0;32m 🌟\033[0m\n";
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
	if (std::chrono::duration_cast<std::chrono::seconds>(now - lastCgiTimeoutCheck) > std::chrono::seconds(CGI_TIMEOUT_CHECK_INTERVAL))
	{
		for (size_t i = 0; i < servers.size(); i++)
			servers[i]->checkForCgiTimeouts();
		lastCgiTimeoutCheck = now;
	}
}

void	ServerManager::processReadEvent(EventInfo &event)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		if (event.fd == servers[i]->_socket)
		{
			Logger::log(Logger::DEBUG, "processReadEvent: New connection on server socket", "EventLoop");
			servers[i]->acceptNewConnection();
			return;
		}
		else if (servers[i]->_clients.count(event.fd) > 0)
		{
			if (event.isEOF)
			{
				Logger::log(Logger::DEBUG, "processReadEvent: Client disconnected", "EventLoop");
				servers[i]->handleClientDisconnection(event.fd);
			}
			else
			{
				Logger::log(Logger::DEBUG, "processReadEvent: Client sent data", "EventLoop");
				servers[i]->handleClientRequest(event.fd);
			}
			return;
		}
		else if (servers[i]->_cgi.count(event.fd) > 0)
		{
			Logger::log(Logger::DEBUG, "processReadEvent For Cgi", "EventLoop");
			servers[i]->handleCgiOutput(event.fd);
			return;
		}
	}
}

void	ServerManager::processWriteEvent(EventInfo &event)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		if (servers[i]->_responses.count(event.fd) > 0)
		{
			Logger::log(Logger::DEBUG, "processWriteEvent: Client is ready to receive data", "EventLoop");
			servers[i]->handleClientResponse(event.fd);
			return;
		}
	}
}

void	ServerManager::start()
{
	this->lastTimeoutCheck = std::chrono::steady_clock::now();
	this->lastCgiTimeoutCheck = std::chrono::steady_clock::now();

	while (running)
	{
		checkTimeouts();

		Logger::log(Logger::DEBUG, "Waiting for events", "EventLoop");

		int nev = eventManager->waitForEvents();
		if (!running)
			break;

		Logger::log(Logger::DEBUG, "Received " + std::to_string(nev) + " events", "EventLoop");

		// update it for epoll
		if (nev < 0)
		{
			if (errno == EINTR)
			{
				Logger::log(Logger::DEBUG, "Interrupted by signal", "EventLoop");
				continue;
			}
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
			EventInfo event;
			eventManager->getNextEvent(ev, event);
			if (event.isRead || event.isEOF)
				processReadEvent(event);
		}

		for (int ev = 0; ev < nev; ev++)
		{
			EventInfo event;
			eventManager->getNextEvent(ev, event);
			if (event.isWrite)
				processWriteEvent(event);
		}
	}

	stop();
}

void	ServerManager::stop()
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		delete servers[i];
	}
}
