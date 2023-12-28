#include "Server.hpp"

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
	std::cout << "Request from client: " << std::endl;
	std::cout << buffer << std::endl;
	
	std::istringstream iss(buffer);
	std::vector<std::string> parsed((std::istream_iterator<std::string>(iss)),
		std::istream_iterator<std::string>());
	if (parsed.size() > 3 && parsed[0] == "GET")
	{
		int statusCode = 200;
		std::string contentType = "text/html";
		std::string content;
		std::string fileName = parsed[1].substr(1);
		
		// check the file extension
		if (fileName.substr(fileName.length() - 4) == ".css")
		{
			contentType = "text/css";
		}
		else if (fileName.substr(fileName.length() - 3) == ".js")
		{
			contentType = "text/javascript";
		}
		else if (fileName.substr(fileName.length() - 4) == ".png")
		{
			contentType = "image/png";
		}
		else if (fileName.substr(fileName.length() - 4) == ".jpg")
		{
			contentType = "image/jpg";
		}
		else if (fileName.substr(fileName.length() - 5) == ".jpeg")
		{
			contentType = "image/jpeg";
		}
		else if (fileName.substr(fileName.length() - 4) == ".gif")
		{
			contentType = "image/gif";
		}
		else if (fileName.substr(fileName.length() - 4) == ".ico")
		{
			contentType = "image/x-icon";
		}
		else if (fileName.substr(fileName.length() - 5) == ".json")
		{
			contentType = "application/json";
		}
		else if (fileName.substr(fileName.length() - 4) == ".pdf")
		{
			contentType = "application/pdf";
		}
		else if (fileName.substr(fileName.length() - 4) == ".svg")
		{
			contentType = "image/svg+xml";
		}
		else if (fileName.substr(fileName.length() - 4) == ".txt")
		{
			contentType = "text/plain";
		}
		else if (fileName.substr(fileName.length() - 5) == ".woff")
		{
			contentType = "font/woff";
		}
		else if (fileName.substr(fileName.length() - 6) == ".woff2")
		{
			contentType = "font/woff2";
		}
		else if (fileName.substr(fileName.length() - 4) == ".xml")
		{
			contentType = "application/xml";
		}
		else if (fileName.substr(fileName.length() - 5) == ".webp")
		{
			contentType = "image/webp";
		}


		if (fileName == "")
		{
			fileName = "index.html";
		}
		std::ifstream file;
		file.open(fileName);
		
		if (file.is_open())
		{
			std::string line;
			
			while (std::getline(file, line))
				content += line;
			file.close();
		}
		else {
			statusCode = 404;
			content = "<h1>404 Not Found</h1>";
		}

		std::string header = "HTTP/1.1 " + std::to_string(statusCode) + " OK\r\nContent-Type: " + contentType + "\r\n";
		// add connection keep-alive header
		header += "Connection: keep-alive\r\n";
		// add content length header
		header += "Content-Length: " + std::to_string(content.length()) + "\r\n";
		header += "\r\n";
		header += content;
		// std::cout << header << std::endl;
		send(clientSocket, header.c_str(), header.length(), 0);
		// close(clientSocket);
	}

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