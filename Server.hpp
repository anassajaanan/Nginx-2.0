


#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfig.hpp"
#include "ResponseState.hpp"
#include "CgiHandler.hpp"


#include <fcntl.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>



#define SERVER_BACKLOG 60

#define BUFFER_SIZE 4096 // 4 KB

#define MAX_REQUEST_HEADERS_SIZE 16384 // 16 KB

#define MAX_URI_SIZE 4096 // 4 KB

#define TEMP_FILE_DIRECTORY "./uploads/"

#define CGI_TIMEOUT 10 // 10 seconds

// define max size of cgi output
#define CGI_MAX_OUTPUT_SIZE 2097152 // 2 MB in bytes


class ClientState;

class Server
{

public:
	Server(ServerConfig &config, EventPoller *eventManager, MimeTypeConfig &mimeTypes);
	~Server();

	ServerConfig						&_config;
	MimeTypeConfig						&_mimeTypes;
	EventPoller							*_eventManager;
	int									_socket;
	struct sockaddr_in					_serverAddr;
	std::map<int, ClientState *>		_clients;
	std::map<int, ResponseState *>		_responses;
	std::map<int, CgiHandler *>			_cgi;

	// Server Creation
	void		createServerSocket();
	void		setSocketOptions();
	void		setSocketToNonBlocking();
	void		bindAndListen();

	void		run();

	// Client Handling
	void		acceptNewConnection();
	void		handleClientDisconnection(int clientSocket);

	// Request Handling
	void		handleClientRequest(int clientSocket);
	void		processGetRequest(int clientSocket, HttpRequest &request);
	void		processPostRequest(int clientSocket, HttpRequest &request, bool closeConnection = false);
	void		processDeleteRequest(int clientSocket, HttpRequest &request);

	// Response Handling
	void		handleClientResponse(int clientSocket);
	void		sendSmallResponse(int clientSocket, ResponseState *responseState);
	void		sendLargeResponse(int clientSocket, ResponseState *responseState);
	void		sendLargeResponseHeaders(int clientSocket, ResponseState *responseState);
	void		sendLargeResponseChunk(int clientSocket, ResponseState *responseState);

	// Error Handling
	void		handleHeaderSizeExceeded(int clientSocket);
	void		handleUriTooLarge(int clientSocket);
	void		handleInvalidGetRequest(int clientSocket);
	void		handleInvalidRequest(int clientSocket, int requestStatusCode, const std::string &detail = "");

	// Timeout and Cleanup
	void		checkForTimeouts();
	void		checkForCgiTimeouts();
	void		removeClient(int clientSocket);

	// Utility
	std::string	getStatusMessage(int statusCode);

	// Handle Cgi
	void		handleCgiOutput(int cgiReadFd);
};

#endif /* SERVER_HPP */
