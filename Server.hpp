


#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfig.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#define SERVER_BACKLOG 30


class Server
{
private:
	ServerConfig		&_config;
	int					_socket;
	int					_kq;
	struct sockaddr_in	_serverAddr;

public:
	Server(ServerConfig &config, int kq);
	~Server();

	void	createServerSocket();
	void	setSocketToNonBlocking();
	void	bindAndListen();




};



#endif /* SERVER_HPP */