#include "Server.hpp"


Server::Server(ServerConfig &config, KqueueManager &kq) : _config(config), _kq(kq)
{
	std::cout << "Server constructor" << std::endl;
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
	std::cout << "Handling client request" << std::endl;
	std::string buffer;
	buffer.resize(1024);
	int bytesReceived = recv(clientSocket, &buffer[0], buffer.size(), 0);
	if (bytesReceived < 0)
		throw std::runtime_error("Error: recv failed");
	buffer.resize(bytesReceived);
	std::cout << "Received: " << buffer << std::endl;

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
	close(_socket);
}