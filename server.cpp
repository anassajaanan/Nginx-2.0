#include <cstddef>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/epoll.h>


#define MAX_EVENTS 64

int main()
{
	struct sockaddr_in			server_addr;
	struct epoll_event			events[MAX_EVENTS];

	try
	{
		int epfd = epoll_create1(0);
		if (epfd < 0)
		{
			throw std::runtime_error("Failed to create epoll");
		}

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

		// register server socket to read
		struct epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = server_fd;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) < 0)
		{
			throw std::runtime_error("epoll_ctl failed");
		}

		while(1)
		{
			int nev = epoll_wait(epfd, events, MAX_EVENTS, 5000);
			std::cout << "nev is : " << nev << std::endl;
			if (nev < 0)
			{
				throw std::runtime_error("epoll_wait failed");
			}
			else if (nev == 0)
			{
				std::cout << "No event" << std::endl;
			}
			else
			{
				for (int ev = 0; ev < nev; ev++)
				{
					if (events[ev].data.fd == server_fd)
					{
						//new connection
						std::cout << "New connection" << std::endl;
						struct sockaddr_in client_addr;
						socklen_t client_len = sizeof(client_addr);

						int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
						if (client_fd < 0)
						{
							throw std::runtime_error("Failed to accept connection");
						}

						struct epoll_event client_event;
						client_event.events = EPOLLIN;
						client_event.data.fd = client_fd;
						if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_event) < 0)
						{
							throw std::runtime_error("epoll_ctl failed");
						}
					}
					else
					{
						std::cout << "Read request from client" << std::endl;
						char buffer[1024];
						int bytes_read = recv(events[ev].data.fd, buffer, 1024, 0);
						if (bytes_read < 0)
						{
							throw std::runtime_error("Failed to read data from client");
						}
						else if (bytes_read == 0)
						{
							std::cout << "Client Disconnected" << std::endl;
							// remove client from epoll
							if (epoll_ctl(epfd, EPOLL_CTL_DEL, events[ev].data.fd, NULL) < 0)
							{
								throw std::runtime_error("epoll_ctl failed");
							}
						}
						else {
							std::cout << "Received: " << std::string(buffer, bytes_read) << std::endl;
						}
					}
				}
			}

		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

}

