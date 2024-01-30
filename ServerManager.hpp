

#pragma once
#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include "Server.hpp"
#include "ServerConfig.hpp"
#include "MimeTypeParser.hpp"

#ifdef __linux__
    // Linux-specific implementation
    #include "EpollManager.hpp"
    typedef EpollManager EventManager;
#elif defined(__APPLE__) || defined(__FreeBSD__)
    // macOS or FreeBSD-specific implementation
    #include "KqueueManager.hpp"
    typedef KqueueManager EventManager;
#endif


#define SERVER_TIMEOUT_CHECK_INTERVAL 5 // 5 seconds
#define CGI_TIMEOUT_CHECK_INTERVAL 10 // 10 seconds

class ServerManager
{
private:
	EventPoller								*eventManager;
	std::chrono::steady_clock::time_point	lastTimeoutCheck;
	std::chrono::steady_clock::time_point	lastCgiTimeoutCheck;
	std::vector<Server *>					servers;


public:
	static int								running;

	ServerManager(std::vector<ServerConfig> &_serverConfigs, EventPoller *_eventManager, MimeTypeConfig &_mimeTypes);
	~ServerManager();
	
	

	void				initializeServers(std::vector<ServerConfig> &serverConfigs, MimeTypeConfig &mimeTypes);
	void 				checkTimeouts();
	void				processReadEvent(EventInfo &event);
	void				processWriteEvent(EventInfo &event);
	std::string			readCgiResponse(int fd);


	void	start();
	void	stop();

};


#endif /* SERVER_MANAGER_HPP */
