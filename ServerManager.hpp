


#pragma once
#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "MimeTypeParser.hpp"
#include "KqueueManager.hpp"

class ServerManager
{
private:
	KqueueManager							kqueue;
	std::vector<Server *>					servers;
	std::chrono::steady_clock::time_point	lastTimeoutCheck;


public:
	static int								running;

	ServerManager(std::vector<ServerConfig> &serverConfigs, MimeTypeParser &mimeTypes);
	~ServerManager();
	
	

	void	initializeServers(std::vector<ServerConfig> &serverConfigs, MimeTypeParser &mimeTypes);

	void	start();

};


#endif /* SERVERMANAGER_HPP */