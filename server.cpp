#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <vector>
#include <sys/select.h>



int main()
{

	try
	{
		fd_set readfds, writefds, exceptfds;
		struct sockaddr_in server_addr;
		std::vector<int> clientSockets;


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

		while (1)
		{
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_ZERO(&exceptfds);

			FD_SET(server_fd, &readfds);
			FD_SET(server_fd, &exceptfds);

			for (size_t i = 0; i < clientSockets.size(); i++)
			{
				FD_SET(clientSockets[i], &readfds);
				FD_SET(clientSockets[i], &exceptfds);
			}

			int max_fd = server_fd;
			for (size_t i = 0; i < clientSockets.size(); i++)
			{
				if (clientSockets[i] > max_fd)
				{
					max_fd = clientSockets[i];
				}
			}
			max_fd++;

			struct timeval timeout;
			timeout.tv_sec = 3;
			timeout.tv_usec = 0;

			int activity = select(max_fd, &readfds, NULL, &exceptfds, &timeout);
			if (activity < 0)
			{
				throw std::runtime_error("Error in select");
			}
			else if (activity == 0)
			{
				std::cout << "Timeout occurred" << std::endl;
			}
			else
			{
				if (FD_ISSET(server_fd, &readfds))
				{
					struct sockaddr_in client_addr;
					socklen_t client_len = sizeof(client_addr);

					int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
					if (client_fd <= 0)
					{
						throw std::runtime_error("Failed to accept incoming connection");
					}
					else
					{
						std::cout << "New connection accepted" << std::endl;
						clientSockets.push_back(client_fd);
					}

				}
				else
				{
					for (size_t i  = 0; i < clientSockets.size(); i++)
					{
						if (FD_ISSET(clientSockets[i], &readfds))
						{
							char buffer[1024] = {0};
							int read_bytes = recv(clientSockets[i], buffer, 1024, 0);
							if (read_bytes < 0)
							{
								throw std::runtime_error("Failed to read from socket");
							}
							else if (read_bytes == 0)
							{
								std::cout << "Connection closed" << std::endl;
								close(clientSockets[i]);
								clientSockets.erase(clientSockets.begin() + i);
								i--;
								continue;
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