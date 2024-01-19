


#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfig.hpp"
#include "KqueueManager.hpp"
#include "MimeTypeParser.hpp"
#include "RequestHandler.hpp"
#include "ResponseState.hpp"


#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>



#define SERVER_BACKLOG 30

#define BUFFER_SIZE 4096 // 4 KB

#define MAX_REQUEST_HEADERS_SIZE 16384 // 16 KB

// define max uri size as 4 KB
#define MAX_URI_SIZE 4096 // 4 KB

#define TEMP_FILE_DIRECTORY "/Users/aajaanan/goinfre/brew/var/uploads/"


class ClientState;

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
	void	sendSmallResponse(int clientSocket, ResponseState *responseState);

	void	checkForTimeouts();

	void	removeClient(int clientSocket);
	void	handleHeaderSizeExceeded(int clientSocket);
	void	handleUriTooLarge(int clientSocket);
	void	handleInvalidGetRequest(int clientSocket);
	void	handleInvalidRequest(int clientSocket, int requestStatusCode, const std::string &detail = "");

	void	processGetRequest(int clientSocket, HttpRequest &request);
	void	processPostRequest(int clientSocket, HttpRequest &request, bool closeConnection = false);

	

	std::string	getStatusMessage(int statusCode);

	void	run();

};


#endif /* SERVER_HPP */