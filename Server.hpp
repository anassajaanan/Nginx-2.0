


#pragma once
#include "HttpRequest.hpp"
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

#define SERVER_TIMEOUT_CHECK_INTERVAL 5 // 5 seconds


#define BUFFER_SIZE 4096 // 4 KB

#define MAX_REQUEST_HEADERS_SIZE 16384 // 16 KB

// define max uri size as 4 KB
#define MAX_URI_SIZE 4096 // 4 KB


// class	ClientState
// {
// private:
	// int													fd;
	// std::chrono::time_point<std::chrono::steady_clock>	lastRequestTime;
	// int													requestCount;

// public:
// 	ClientState(int fd);
	
	// void	updateLastRequestTime();
	// void	incrementRequestCount();

	// int		getFd() const;
	// bool	isTimedOut(size_t keepalive_timeout) const;
	// int		getRequestCount() const;


// };	

class ClientState
{
private:
	int													fd;
	std::chrono::time_point<std::chrono::steady_clock>	lastRequestTime;
	int													requestCount;



public:
	ClientState(int fd);
	~ClientState();

	HttpRequest		*request;
	std::string		requestHeaders;
	std::ofstream	requestBodyFile;
	std::string		requestBodyFilePath;
	bool			areHeaderComplete;
	bool			isBodyComplete;
	bool			isChunked;



	void	updateLastRequestTime();
	void	incrementRequestCount();

	int		getFd() const;
	bool	isTimedOut(size_t keepalive_timeout) const;
	int		getRequestCount() const;


	void processIncomingData(const char *buffer, int bytesRead);
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

	void	removeClient(int clientSocket);
	void	handleHeaderSizeExceeded(int clientSocket);


	void	run();
	void	stop();

};



#endif /* SERVER_HPP */