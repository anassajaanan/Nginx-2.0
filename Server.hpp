


#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfig.hpp"
#include "KqueueManager.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>




#define SERVER_BACKLOG 30


class Server
{
private:


public:
	Server(ServerConfig &config, KqueueManager &kq);
	~Server();

	ServerConfig						&_config;
	KqueueManager						&_kq;
	int									_socket;
	struct sockaddr_in					_serverAddr;
	int									_running;
	std::map<int, struct sockaddr_in>	_clients;



	void	createServerSocket();
	void	setSocketToNonBlocking();
	void	bindAndListen();
	void	acceptNewConnection();
	void	handleClientDisconnection(int clientSocket);
	void	handleClientRequest(int clientSocket);


	void	run();
	void	stop();

};



#endif /* SERVER_HPP */