


#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>

#include "ServerConfig.hpp"


class Server
{
private:
	ServerConfig	&config;
	int				socket;
	static int		kq;

public:
	Server(ServerConfig &config, int kq);
	~Server();










};



#endif /* SERVER_HPP */