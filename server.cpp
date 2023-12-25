#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <sys/select.h>
#include <poll.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

int main()
{

	try
	{
		struct sockaddr_in			server_addr;
		std::vector<struct pollfd>	clientSockets;


		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd < 0)
		{
			throw std::runtime_error("Failed to create socket");
		}

		// Set socket to non-blocking
		int flags = fcntl(server_fd, F_GETFL, 0);
		if (flags < 0)
		{
			throw std::runtime_error("Failed to get socket flags");
		}
		if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) < 0)
		{
			throw std::runtime_error("Failed to set socket to non-blocking");
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(9090);
		server_addr.sin_addr.s_addr = INADDR_ANY;
		memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

		if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		{
			throw std::runtime_error("Failed to bind socket to port");
		}

		if (listen(server_fd, 20) < 0)
		{
			throw std::runtime_error("Failed to listen on socket");
		}

		// add server socket to pollfd vector
		struct pollfd serverPollfd;

		serverPollfd.fd = server_fd;
		serverPollfd.events = POLLIN;
		serverPollfd.revents = 0;

		clientSockets.push_back(serverPollfd);

		while (1)
		{
			int activity = poll(clientSockets.data(), clientSockets.size(), 3000);
			if (activity < 0)
			{
				throw (std::runtime_error("Failed to poll sockets"));
			}
			else if (activity == 0){
				std::cout << "Timeout" << std::endl;
			}
			else
			{
				for (int fd = 0; fd < clientSockets.size(); fd++)
				{
					if ((clientSockets[fd].revents & POLLIN) == POLLIN)
					{
						if (clientSockets[fd].fd == server_fd) // new request
						{
							struct sockaddr_in client_addr;
							socklen_t client_addr_len = sizeof(client_addr);
							int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
							if (client_fd <= 0)
							{
								throw std::runtime_error("Failed to accept connection");
							}

							std::cout << "New connection accepted" << std::endl;

							struct pollfd clientPollfd;
							clientPollfd.fd = client_fd;
							clientPollfd.events = POLLIN;
							clientPollfd.revents = 0;
							clientSockets.push_back(clientPollfd);
						}
						else {
							char buffer[1024] = { 0 };
							int readBytes = recv(clientSockets[fd].fd, buffer, 1024, 0);
							if (readBytes < 0)
							{
								throw std::runtime_error("Failed to read from socket");
							}
							else if (readBytes == 0)
							{
								std::cout << "Client disconnected" << std::endl;
								close(clientSockets[fd].fd);
								clientSockets.erase(clientSockets.begin() + fd);
							}
							else
							{
								std::cout << "Received: " << buffer << std::endl;
							}
						}
					}
				}
			}

		}
		
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}