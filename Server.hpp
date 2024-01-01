


#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfig.hpp"
#include "KqueueManager.hpp"
#include "MimeTypeParser.hpp"
#include "RequestHandler.hpp"


#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>




#define SERVER_BACKLOG 30

#define SERVER_TIMEOUT 15

// periodic timeout checking 
#define SERVER_TIMEOUT_CHECK_INTERVAL 5


class	ClientState
{
private:
	int													fd;
	std::chrono::time_point<std::chrono::steady_clock>	lastRequestTime;
	int													requestCount;

public:
	ClientState(int fd);
	
	void	updateLastRequestTime();
	void	incrementRequestCount();

	int		getFd() const;
	bool	isTimedOut() const;
	int		getRequestCount() const;


};	


class Server
{
private:


public:
	Server(ServerConfig &config, MimeTypeParser &mimeTypes, KqueueManager &kq);
	~Server();

	ServerConfig						&_config;
	MimeTypeParser						&_mimeTypes;
	KqueueManager						&_kq;
	int									_socket;
	struct sockaddr_in					_serverAddr;
	int									_running;
	std::map<int, ClientState *>		_clients;
	std::map<int, ResponseState *>		_responses;



	void	createServerSocket();
	void	setSocketOptions();
	void	setSocketToNonBlocking();
	void	bindAndListen();
	void	acceptNewConnection();
	void	handleClientDisconnection(int clientSocket);
	void	handleClientRequest(int clientSocket);
	void	handleClientResponse(int clientSocket);

	void	checkForTimeouts();


	void	run();
	void	stop();

};



#endif /* SERVER_HPP */