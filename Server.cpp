#include "Server.hpp"
#include "ClientState.hpp"
#include "Logger.hpp"

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
	std::cerr << "Processing post request" << std::endl;
	ResponseState *responseState;

	RequestHandler handler(_config, _mimeTypes);
	HttpResponse response = handler.handleRequest(request);

	_clients[clientSocket]->resetClientState();
	

	responseState = new ResponseState(response.buildResponse(), closeConnection);

	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);

	std::cerr << "Post Request is processed" << std::endl;
}

void	Server::handleHeaderSizeExceeded(int clientSocket)
{
	HttpResponse response;

	removeClient(clientSocket);
	response.generateStandardErrorResponse("400", "Bad Request", "400 Request Header Or Cookie Too Large", "Request Header Or Cookie Too Large");
	ResponseState *responseState = new ResponseState(response.buildResponse(), true);
	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);
}

void	Server::handleUriTooLarge(int clientSocket)
{
	HttpResponse response;

	removeClient(clientSocket);
	response.generateStandardErrorResponse("414", "Request-URI Too Large", "414 Request-URI Too Large");
	ResponseState *responseState = new ResponseState(response.buildResponse(), true);
	_responses[clientSocket] = responseState;
	_kq.registerEvent(clientSocket, EVFILT_WRITE);
}

void	Server::handleInvalidGetRequest(int clientSocket)
{
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

void	Server::handleClientResponse(int clientSocket)
{
	if (_responses.count(clientSocket) == 0)
	{
		std::cerr << "Error: no response state found for client socket " << clientSocket << std::endl;
		_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
		return;
	}
	std::cout << "Handling client response" << std::endl;

	ResponseState *responseState = _responses[clientSocket];

	if (responseState->getType() == SMALL_RESPONSE)
	{
		std::cout << "Sending small file" << std::endl;
		const std::string &response = responseState->getSmallResponse();

		int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
		if (bytesSent < 0)
			throw std::runtime_error("Error: send failed");
		if (bytesSent == (int)response.length())
		{
			std::cerr << "Small file sent completely" << std::endl;
			_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
			_responses.erase(clientSocket);
			if (responseState->closeConnection)
			{
				std::cerr << "Closing connection after sending small response" << std::endl;
				close(clientSocket);
			}
			delete responseState;
		}
	}
	else if (responseState->getType() == LARGE_RESPONSE)
	{
		// send headers first, then send file in chunks
		if (!responseState->isHeaderSent)
		{
			std::cout << "Sending headers ..." << std::endl;
			const std::string &headers = responseState->getHeaders();
			int bytesSent = send(clientSocket, headers.c_str(), headers.length(), 0);
			if (bytesSent < 0)
			{
				if (errno == EPIPE)
				{
					std::cerr << "Client disconnected (Broken Pipe)" << std::endl;
				}
				else
				{
					std::cerr << "Error: send failed, errno: " << errno << std::endl;
				}
				_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
				_responses.erase(clientSocket);
				delete responseState;
				close(clientSocket);
			}
			if (bytesSent == (int)headers.length())
				responseState->isHeaderSent = true;
		}
		else
		{
			std::cout << "Sending chunks ..." << std::endl;
			// send file in chunks
			std::string chunk = responseState->getNextChunk();

			std::cout << "start sending ..." << std::endl;

			// convert chunk size to hex
			std::stringstream ss;
			ss << std::hex << chunk.length();
			std::string chunkSizeHex = ss.str();

			std::string formattedChunk = chunkSizeHex + "\r\n" + chunk + "\r\n";
			std::cout << "before send" << std::endl;
			int bytesSent = send(clientSocket, formattedChunk.c_str(), formattedChunk.length(), 0);
			std::cout << "after send" << std::endl;
			if (bytesSent < 0)
			{
				if (errno == EPIPE)
				{
					std::cerr << "Client disconnected (Broken Pipe)" << std::endl;
				}
				else
				{
					std::cerr << "Error: send failed, errno: " << errno << std::endl;
				}
				_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
				_responses.erase(clientSocket);
				delete responseState;
				close(clientSocket);
			}
			if (bytesSent == (int)formattedChunk.length())
			{
				std::cout << "Chunk sent completely !" << std::endl;
				if (responseState->isFinished())
				{
					std::cout << "Sending end chunk" << std::endl;
					std::string endChunk = "0\r\n\r\n";
					int bytesSent = send(clientSocket, endChunk.c_str(), endChunk.length(), 0);
					if (bytesSent < 0)
					{
						if (errno == EPIPE)
						{
							std::cerr << "Client disconnected (Broken Pipe)" << std::endl;
						}
						else
						{
							std::cerr << "Error: send failed, errno: " << errno << std::endl;
						}
						_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
						_responses.erase(clientSocket);
						delete responseState;
						close(clientSocket);
					}
					_kq.unregisterEvent(clientSocket, EVFILT_WRITE);
					_responses.erase(clientSocket);
					delete responseState;
				}
			}
			else {
				std::cout << "Chunk not sent completely !" << std::endl;
			}
		}
	}
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