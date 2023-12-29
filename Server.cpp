#include "Server.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "RequestHandler.hpp"
#include <unistd.h>

Server::Server(ServerConfig &config, KqueueManager &kq) : _config(config), _kq(kq)
{
	// std::cout << "Server constructor" << std::endl;
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_config.port);
	_serverAddr.sin_addr.s_addr = inet_addr(_config.ipAddress.c_str());
	memset(_serverAddr.sin_zero, '\0', sizeof(_serverAddr.sin_zero));
}

Server::~Server()
{
	std::cout << "Server destructor" << std::endl;
	std::map<int, struct sockaddr_in>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		_kq.unregisterEvent(it->first, EVFILT_READ);
		close(it->first);
		it++;
	}
	_clients.clear();
	_kq.unregisterEvent(_socket, EVFILT_READ);
	close(_socket);
}

void	Server::createServerSocket()
{
	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket < 0)
		throw std::runtime_error("Error: socket creation failed");
}


void	Server::setSocketToNonBlocking()
{
	int flags = fcntl(this->_socket, F_GETFL, 0);
	if (flags < 0)
		throw std::runtime_error("Error: fcntl failed");
	if (fcntl(this->_socket, F_SETFL, flags | O_NONBLOCK) < 0)
		throw std::runtime_error("Error: fcntl failed");
}

void	Server::bindAndListen()
{
	if (bind(this->_socket, (struct sockaddr *)&this->_serverAddr, sizeof(this->_serverAddr)) < 0)
		throw std::runtime_error("Error: bind failed");
	if (listen(this->_socket, SERVER_BACKLOG) < 0)
		throw std::runtime_error("Error: listen failed");
}


void	Server::acceptNewConnection()
{
	std::cout << "Accepting new connection" << std::endl;
	struct sockaddr_in	clientAddr;
	socklen_t			clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(this->_socket, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if (clientSocket < 0)
		throw std::runtime_error("Error: accept failed");
	_clients[clientSocket] = clientAddr;
	_kq.registerEvent(clientSocket, EVFILT_READ);
}

void	Server::handleClientDisconnection(int clientSocket)
{
	std::cout << "Handling client disconnection" << std::endl;
	_kq.unregisterEvent(clientSocket, EVFILT_READ);
	_clients.erase(clientSocket);
	close(clientSocket);
}

void	Server::handleClientRequest(int clientSocket)
{
	// std::cout << "Handling client request" << std::endl;
	// std::string buffer;
	// buffer.resize(1024);
	// int bytesReceived = recv(clientSocket, &buffer[0], buffer.size(), 0);
	// if (bytesReceived < 0)
	// 	throw std::runtime_error("Error: recv failed");
	// buffer.resize(bytesReceived);
	// std::cout << "Received: " << buffer << std::endl;

	// std::cout << "Incoming connection accepted" << std::endl;
	// clientSockets.push_back(clientSocket);
	// std::string message = "Hello from server";
	// send(clientSocket, message.c_str(), message.length(), 0);
	

	// read request from an http client (web browser)
	char buffer[1024];
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead < 0)
		throw std::runtime_error("Error: recv failed");
	buffer[bytesRead] = '\0';
	
	RequestHandler handler(_config);

	HttpResponse res = handler.handleRequest(HttpRequest(buffer));
	std::string response = res.buildResponse();
	send(clientSocket, response.c_str(), response.length(), 0);
	close(clientSocket);

}


void	Server::run()
{
	createServerSocket();
	setSocketToNonBlocking();
	bindAndListen();
	_kq.registerEvent(_socket, EVFILT_READ);
	_running = true;
}

void	Server::stop()
{
	this->_running = false;
	std::map<int, struct sockaddr_in>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		_kq.unregisterEvent(it->first, EVFILT_READ);
		close(it->first);
		it++;
	}
	_clients.clear();
}