


#pragma once
#include "HttpRequest.hpp"
#include <string>
#include <sys/_types/_size_t.h>
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

#define TEMP_FILE_DIRECTORY "/Users/aajaanan/goinfre/brew/var/uploads/"


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

class Server;

class ClientState
{
private:
	int													fd;
	std::chrono::time_point<std::chrono::steady_clock>	lastRequestTime;
	int													requestCount;



public:
	ClientState(int fd);
	~ClientState();

	HttpRequest		request;
	std::string		requestHeaders;
	std::string		requestBody;
	std::ofstream	requestBodyFile;
	size_t			requestBodySize;
	std::string		requestBodyFilePath;
	bool			areHeaderComplete;
	bool			isBodyComplete;
	// bool			isChunked;



	void	updateLastRequestTime();
	void	incrementRequestCount();

	int		getFd() const;
	bool	isTimedOut(size_t keepalive_timeout) const;
	int		getRequestCount() const;

	bool	headersCompleted(const char *buffer) const;

	void	initializeBodyStorage(Server &server);

	void	handleGetRequest(Server &server);
	void	handlePostRequest(Server &server);

	void	parseHeaders(Server &server);

	void	processHeaders(Server &server, const char *buffer, size_t bytesRead);

	void	processBody(Server &server, const char *buffer, size_t bytesRead);


	void 	processIncomingData(Server &server, const char *buffer, size_t bytesRead);

	void	resetClientState();
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
	void	handleUriTooLarge(int clientSocket);
	void	handleInvalidGetRequest(int clientSocket);
	void	handleInvalidRequest(int clientSocket, int requestStatusCode);

	void	processGetRequest(int clientSocket, HttpRequest &request);
	void	processPostRequest(int clientSocket, HttpRequest &request, bool closeConnection = false);

	

	std::string	getStatusMessage(int statusCode);

	void	run();
	void	stop();

};



#endif /* SERVER_HPP */