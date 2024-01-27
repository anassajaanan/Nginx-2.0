


#pragma once
#include "HttpRequest.hpp"
#include <sys/_types/_pid_t.h>
#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfig.hpp"
#include "KqueueManager.hpp"
#include "MimeTypeParser.hpp"
#include "RequestHandler.hpp"
#include "ResponseState.hpp"
#include "CgiHandler.hpp"


#include <fcntl.h>
#include <fstream>
#include <unistd.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>



#define SERVER_BACKLOG 60

#define BUFFER_SIZE 4096 // 4 KB

#define MAX_REQUEST_HEADERS_SIZE 16384 // 16 KB

#define MAX_URI_SIZE 4096 // 4 KB

#define TEMP_FILE_DIRECTORY "./uploads/"


class ClientState;

class CgiState;

class Server
{

public:
	Server(ServerConfig &config, MimeTypeConfig &mimeTypes, KqueueManager &kq);
	~Server();

	ServerConfig						&_config;
	MimeTypeConfig						&_mimeTypes;
	KqueueManager						&_kq;
	int									_socket;
	struct sockaddr_in					_serverAddr;
	std::map<int, ClientState *>		_clients;
	std::map<int, ResponseState *>		_responses;
	std::map<int, CgiState *>			_cgiStates;

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
	void		removeClient(int clientSocket);

	// Utility
	std::string	getStatusMessage(int statusCode);

	//cgi Utility
	bool			validCgiRequest(HttpRequest &request, ServerConfig &config);
	bool			validateFileExtension(HttpRequest &request);
	bool			fileExists(const std::string &path);



	void	handleCgiRequest(int clientSocket, HttpRequest &request);
	void	handleCgiOutput(int pipeReadFd);


};


class CgiState
{

private:


public:
	CgiState(pid_t childPid, int pipeReadFd, int clientSocket);
	
	pid_t		_pid;
	int			_pipeReadFd;
	int			_clientSocket;
	std::string	_cgiResponseMessage;

};


#endif /* SERVER_HPP */
