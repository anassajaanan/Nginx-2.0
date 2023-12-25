#include <cstddef>
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

#define MAX_EVENTS 64

int main()
{

	try
	{
		struct sockaddr_in			server_addr;
		struct kevent				events[MAX_EVENTS];

		int kq = kqueue();
		if (kq < 0)
		{
			throw std::runtime_error("Failed to create kqueue");
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

		struct kevent server_event;
		EV_SET(&server_event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(kq, &server_event, 1, NULL, 0, NULL) < 0)
		{
			throw std::runtime_error("Failed to add server socket to kqueue");
		}


		while (1)
		{
			// int activity = poll(clientSockets.data(), clientSockets.size(), 3000);
			int nev = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
			std::cout << "nev: " << nev << std::endl;
			if (nev < 0)
			{
				throw std::runtime_error("Failed to check for events");
			}
			for (int i = 0; i < nev; i++)
			{
				if (events[i].filter == EVFILT_READ)
				{
					if (events[i].ident == server_fd)
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

						// register new client
						struct kevent new_client_event;
						EV_SET(&new_client_event, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
						if (kevent(kq, &new_client_event, 1, NULL, 0, NULL) < 0)
						{
							throw std::runtime_error("Failed to add new client to kqueue");
						}

					}
					else {
						// read from client
						char buffer[1024];
						int bytes_read = recv(events[i].ident, buffer, sizeof(buffer), 0);
						if (bytes_read < 0)
						{
							throw std::runtime_error("Failed to read data from client");
						}
						else if (bytes_read == 0)
						{
							// client disconnected
							std::cout << "Client disconnected" << std::endl;
							// remove client from kqueue
							struct kevent del_client_event;
							EV_SET(&del_client_event, events[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
							if (kevent(kq, &del_client_event, 1, NULL, 0, NULL) < 0)
							{
								throw std::runtime_error("Failed to delete client from kqueue");
							}
							close(events[i].ident);
						}
						else
							std::cout << "Received: " << std::string(buffer, bytes_read) << std::endl;
						
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