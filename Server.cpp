#include "Server.hpp"
#include "ClientState.hpp"
#include "Logger.hpp"
#include <sys/_types/_size_t.h>

Server::Server(ServerConfig &config, MimeTypeParser &mimeTypes, KqueueManager &kq) : _config(config), _mimeTypes(mimeTypes), _kq(kq), _socket(-1)
{
	// std::cout << "Server constructor" << std::endl;
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_config.port);
	_serverAddr.sin_addr.s_addr = inet_addr(_config.ipAddress.c_str());
	memset(_serverAddr.sin_zero, '\0', sizeof(_serverAddr.sin_zero));
}

Server::~Server()
{
	std::map<int, ClientState *>::iterator client = _clients.begin();
	while (client != _clients.end())
	{
		_kq.unregisterEvent(client->first, EVFILT_READ);
		delete client->second;
		close(client->first);
		client++;
	}
	_clients.clear();

	std::map<int, ResponseState *>::iterator response = _responses.begin();
	while (response != _responses.end())
	{
		_kq.unregisterEvent(response->first, EVFILT_WRITE);
		delete response->second;
		response++;
	}
	_responses.clear();

	if (_socket != -1)
	{
		_kq.unregisterEvent(_socket, EVFILT_READ);
		close(_socket);
	}
}

void	Server::createServerSocket()
{
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket < 0)
	{
		Logger::log(Logger::ERROR, "Failed to create server socket: " + std::string(strerror(errno)), "Server::createServerSocket");
		this->_socket = -1;
	}
	else
		Logger::log(Logger::INFO, "Server socket created successfully", "Server::createServerSocket");
}

void	Server::setSocketOptions()
{
	if (_socket == -1)
		return;

	int opt = 1;
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		Logger::log(Logger::ERROR, "Failed to set socket options: " + std::string(strerror(errno)), "Server::setSocketOptions");
		_socket = -1;
	}
	else
		Logger::log(Logger::INFO, "Socket options set successfully", "Server::setSocketOptions");
}


void	Server::setSocketToNonBlocking()
{
	if (_socket == -1)
		return;

	int flags = fcntl(this->_socket, F_GETFL, 0);
	if (flags < 0)
	{
		Logger::log(Logger::ERROR, "fcntl(F_GETFL) failed: " + std::string(strerror(errno)), "Server::setSocketToNonBlocking");
        _socket = -1;
		return;
	}
	if (fcntl(this->_socket, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		Logger::log(Logger::ERROR, "fcntl(F_SETFL) failed: " + std::string(strerror(errno)), "Server::setSocketToNonBlocking");
		_socket = -1;
	}
	else
		Logger::log(Logger::INFO, "Socket set to non-blocking mode successfully", "Server::setSocketToNonBlocking");
}

void	Server::bindAndListen()
{
	if (_socket == -1)
		return;

	if (bind(this->_socket, (struct sockaddr *)&this->_serverAddr, sizeof(this->_serverAddr)) < 0)
	{
		Logger::log(Logger::ERROR, "Failed to bind socket: " + std::string(strerror(errno)), "Server::bindAndListen");
		_socket = -1;
		return;
	}
	Logger::log(Logger::INFO, "Socket bound successfully", "Server::bindAndListen");
	if (listen(this->_socket, SERVER_BACKLOG) < 0)
	{
		Logger::log(Logger::ERROR, "Failed to listen on socket: " + std::string(strerror(errno)), "Server::bindAndListen");
		_socket = -1;
	}
	else
		Logger::log(Logger::INFO, "Server is now listening on socket", "Server::bindAndListen");
}


void	Server::acceptNewConnection()
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(this->_socket, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSocket < 0)
	{
		Logger::log(Logger::ERROR, "Error accepting new connection: " + std::string(strerror(errno)), "Server::acceptNewConnection");
		return;
	}
	Logger::log(Logger::INFO, "Accepted new connection on socket fd " + std::to_string(clientSocket), "Server::acceptNewConnection");
	ClientState *clientState = new ClientState(clientSocket);
	_clients[clientSocket] = clientState;
	_kq.registerEvent(clientSocket, EVFILT_READ);
}

void	Server::handleClientDisconnection(int clientSocket)
{
	Logger::log(Logger::INFO, "Handling disconnection of client with socket fd " + std::to_string(clientSocket), "Server::handleClientDisconnection");

	_kq.unregisterEvent(clientSocket, EVFILT_READ);
	ClientState *clientState = _clients[clientSocket];
	_clients.erase(clientSocket);
	delete clientState;
	if (_responses.count(clientSocket) > 0)
	{
		_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
		ResponseState *responseState = _responses[clientSocket];
		_responses.erase(clientSocket);
		delete responseState;
	}
	close(clientSocket);
}

void	Server::handleClientRequest(int clientSocket)
{
	char buffer[BUFFER_SIZE];
	size_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);
	if (bytesRead < 0)
	{
		Logger::log(Logger::ERROR, "Error receiving data from client with socket fd " + std::to_string(clientSocket), "Server::handleClientRequest");
		removeClient(clientSocket);
		close(clientSocket);
	}

	if (bytesRead > 0)
	{
		ClientState *client = _clients[clientSocket];

		client->updateLastRequestTime();
		client->incrementRequestCount();

		Logger::log(Logger::DEBUG, "Received new request from client with socket fd " + std::to_string(clientSocket), "Server::handleClientRequest");
		client->processIncomingData(*this, buffer, bytesRead);
	}
}

void	Server::processGetRequest(int clientSocket, HttpRequest &request)
{
	ResponseState *responseState;
	RequestHandler handler(_config, _mimeTypes);
	HttpResponse response = handler.handleRequest(request);
	_clients[clientSocket]->resetClientState();
	
	if (response.getType() == SMALL_RESPONSE)
	{
		Logger::log(Logger::DEBUG, "Generating small response for client with socket fd " + std::to_string(clientSocket), "Server::processGetRequest");
		responseState = new ResponseState(response.buildResponse());
	}
	else
	{
		Logger::log(Logger::DEBUG, "Generating large response for client with socket fd " + std::to_string(clientSocket), "Server::processGetRequest");
		responseState = new ResponseState(response.buildResponse(), response.filePath, response.fileSize);
	}

	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);
}

void	Server::processPostRequest(int clientSocket, HttpRequest &request, bool closeConnection)
{
	Logger::log(Logger::INFO, "Processing POST request for client with socket fd " + std::to_string(clientSocket), "Server::processPostRequest");

	ResponseState *responseState;
	RequestHandler handler(_config, _mimeTypes);
	HttpResponse response = handler.handleRequest(request);
	_clients[clientSocket]->resetClientState();
	
	responseState = new ResponseState(response.buildResponse(), closeConnection);

	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);

	Logger::log(Logger::DEBUG, "POST request processed and response ready for client with socket fd " + std::to_string(clientSocket), "Server::processPostRequest");
}

void	Server::handleHeaderSizeExceeded(int clientSocket)
{
	Logger::log(Logger::WARN, "Request headers size exceeded the maximum limit for fd " + std::to_string(clientSocket), "Server::handleHeaderSizeExceeded");

	HttpResponse response;

	removeClient(clientSocket);
	response.generateStandardErrorResponse("400", "Bad Request", "400 Request Header Or Cookie Too Large", "Request Header Or Cookie Too Large");
	ResponseState *responseState = new ResponseState(response.buildResponse(), true);
	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);
}

void	Server::handleUriTooLarge(int clientSocket)
{
	Logger::log(Logger::WARN, "URI size exceeded the maximum limit for fd " + std::to_string(clientSocket), "Server::handleUriTooLarge");
	
	HttpResponse response;

	removeClient(clientSocket);
	response.generateStandardErrorResponse("414", "Request-URI Too Large", "414 Request-URI Too Large");
	ResponseState *responseState = new ResponseState(response.buildResponse(), true);
	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);
}

void	Server::handleInvalidGetRequest(int clientSocket)
{
	Logger::log(Logger::WARN, "GET request with body received for fd " + std::to_string(clientSocket), "Server::handleInvalidGetRequest");
	
	HttpResponse response;

	removeClient(clientSocket);
	response.generateStandardErrorResponse("400", "Bad Request", "400 Invalid GET Request (with body indicators)", "Invalid GET Request (with body indicators)");
	ResponseState *responseState = new ResponseState(response.buildResponse(), true);
	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);
}


void	Server::handleInvalidRequest(int clientSocket, int requestStatusCode, const std::string &detail)
{
	std::string statusCode = std::to_string(requestStatusCode);
	std::string statusMessage = getStatusMessage(requestStatusCode);

	HttpResponse response;
	removeClient(clientSocket);
	response.generateStandardErrorResponse(statusCode, statusMessage, statusCode + " " + statusMessage, detail);
	ResponseState *responseState = new ResponseState(response.buildResponse(), true);
	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);
}

void	Server::sendSmallResponse(int clientSocket, ResponseState *responseState)
{
	Logger::log(Logger::DEBUG, "Sending small response to client with socket fd " + std::to_string(clientSocket), "Server::sendSmallResponse");

	const std::string &response = responseState->getSmallResponse();
	size_t totalLength = response.length();
	size_t remainingLength = totalLength - responseState->bytesSent;
	const char *responsePtr = response.c_str() + responseState->bytesSent;

	size_t bytesSent = send(clientSocket, responsePtr, remainingLength, 0);

	if (bytesSent < 0)
	{
		Logger::log(Logger::ERROR, "Failed to send small response to client with socket fd " + std::to_string(clientSocket) + ". Error: " + strerror(errno), "Server::sendSmallResponse");
		_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
		_responses.erase(clientSocket);
		delete responseState;
	}
	else
	{
		responseState->bytesSent += bytesSent;
		if (responseState->isFinished())
		{
			Logger::log(Logger::DEBUG, "Small response sent completely to client with socket fd " + std::to_string(clientSocket), "Server::sendSmallResponse");
			_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
			_responses.erase(clientSocket);
			if (responseState->closeConnection)
			{
				Logger::log(Logger::INFO, "Closing connection after sending small response to client with socket fd " + std::to_string(clientSocket), "Server::sendSmallResponse");
				close(clientSocket);
			}
			delete responseState;
		}
		else
			Logger::log(Logger::DEBUG, "Partial small response sent", "Server::sendSmallResponse");
	}
}

void	Server::sendLargeResponseHeaders(int clientSocket, ResponseState *responseState)
{
	Logger::log(Logger::DEBUG, "Sending large response headers to client with socket fd " + std::to_string(clientSocket), "Server::sendLargeResponseHeaders");

	const std::string &headers = responseState->getHeaders();
	size_t totalLength = headers.length();
	size_t remainingLength = totalLength - responseState->headersSent;
	const char *headersPtr = headers.c_str() + responseState->headersSent;

	size_t bytesSent = send(clientSocket, headersPtr, remainingLength, 0);
	if (bytesSent < 0)
	{
		Logger::log(Logger::ERROR, "Failed to send large response headers to client with socket fd " + std::to_string(clientSocket) + ". Error: " + strerror(errno), "Server::sendLargeResponseHeaders");
		_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
		_responses.erase(clientSocket);
		delete responseState;
	}
	else
	{
		responseState->headersSent += bytesSent;
		if (responseState->headersSent >= totalLength)
		{
			Logger::log(Logger::DEBUG, "Large response headers sent completely to client with socket fd " + std::to_string(clientSocket), "Server::sendLargeResponseHeaders");
			responseState->isHeaderSent = true;
		}
		else
			Logger::log(Logger::DEBUG, "Partial large response headers sent", "Server::sendLargeResponseHeaders");
	}
}

void	Server::sendLargeResponseChunk(int clientSocket, ResponseState *responseState)
{
	Logger::log(Logger::DEBUG, "Sending Large Response chunk to client with socket fd " + std::to_string(clientSocket), "Server::sendLargeResponseChunk");

	std::string chunk = responseState->getNextChunk();
	size_t bytesSent = send(clientSocket, chunk.c_str(), chunk.length(), 0);
	if (bytesSent < 0)
	{
		Logger::log(Logger::ERROR, "Failed to send Large Response chunk to client with socket fd " + std::to_string(clientSocket) + ". Error: " + strerror(errno), "Server::sendLargeResponseChunk");
		_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
		_responses.erase(clientSocket);
		delete responseState;
	}
	else
	{
		if (bytesSent == chunk.size())
		{
			Logger::log(Logger::DEBUG, "Chunk sent completely to client with socket fd " + std::to_string(clientSocket), "Server::sendLargeResponseChunk");
            if (responseState->isFinished())
			{
				// send end chunk
				std::string endChunk = "0\r\n\r\n";
				int bytesSent = send(clientSocket, endChunk.c_str(), endChunk.length(), 0);
				if (bytesSent < 0)
				{
					Logger::log(Logger::ERROR, "Failed to send end chunk to client with socket fd " + std::to_string(clientSocket) + ". Error: " + strerror(errno), "Server::sendLargeResponseChunk");
					_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
					_responses.erase(clientSocket);
					delete responseState;
				}
				else
				{
					Logger::log(Logger::DEBUG, "End chunk sent completely to client with socket fd " + std::to_string(clientSocket), "Server::sendLargeResponseChunk");
					_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
					_responses.erase(clientSocket);
					delete responseState;
				}
			}
		}
		else
			Logger::log(Logger::DEBUG, "❌ Chunk not sent completely ❌ ", "Server::sendLargeResponseChunk");
	}
}

void	Server::sendLargeResponse(int clientSocket, ResponseState *responseState)
{
	if (!responseState->isHeaderSent)
		sendLargeResponseHeaders(clientSocket, responseState);
	else
		sendLargeResponseChunk(clientSocket, responseState);
}

void	Server::handleClientResponse(int clientSocket)
{
	if (_responses.count(clientSocket) == 0)
	{
		Logger::log(Logger::ERROR, "No response state found for client socket " + std::to_string(clientSocket), "Server::handleClientResponse");
		_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
		return;
	}

	Logger::log(Logger::DEBUG, "Handling client response for socket " + std::to_string(clientSocket), "Server::handleClientResponse");
	ResponseState *responseState = _responses[clientSocket];

	if (responseState->getType() == SMALL_RESPONSE)
		sendSmallResponse(clientSocket, responseState);
	else if (responseState->getType() == LARGE_RESPONSE)
		sendLargeResponse(clientSocket, responseState);
}

void	Server::checkForTimeouts()
{
	std::map<int, ClientState *>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		if (it->second->isTimedOut(this->_config.keepalive_timeout))
		{
			Logger::log(Logger::INFO, "Client with socket fd " + std::to_string(it->first) + " timed out and is being disconnected", "Server::checkForTimeouts");

			_kq.unregisterEvent(it->first, EVFILT_READ);
			close(it->first);
			delete it->second;
			it = _clients.erase(it);
		}
		else
			it++;
	}
}

void	Server::removeClient(int clientSocket)
{
	if (_clients.find(clientSocket) == _clients.end())
	{
        Logger::log(Logger::WARN, "Attempted to remove non-existent client with socket fd " + std::to_string(clientSocket), "Server::removeClient");
        return;
    }

	Logger::log(Logger::INFO, "Removing client with socket fd " + std::to_string(clientSocket), "Server::removeClient");

	ClientState *clientState = _clients[clientSocket];
	_kq.unregisterEvent(clientSocket, EVFILT_READ);
	_clients.erase(clientSocket);
	delete clientState;
}

std::string	Server::getStatusMessage(int statusCode)
{
	std::map<int, std::string>			statusCodeMessages;

	statusCodeMessages[400] = "Bad Request";
	statusCodeMessages[411] = "Length Required";
	statusCodeMessages[413] = "Request Entity Too Large";
	statusCodeMessages[414] = "Request-URI Too Large";
	statusCodeMessages[500] = "Internal Server Error";
	statusCodeMessages[501] = "Not Implemented";
	statusCodeMessages[505] = "HTTP Version Not Supported";

	if (statusCodeMessages.count(statusCode) == 0)
		return "";
	return statusCodeMessages[statusCode];
}


void	Server::run()
{
	createServerSocket();
	setSocketOptions();
	setSocketToNonBlocking();
	bindAndListen();
}