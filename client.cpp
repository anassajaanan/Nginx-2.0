#include <cstdlib>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>



int main()
{
	struct sockaddr_in server_addr;

	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9090);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(&server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));


	if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Failed to connect to server");
		exit(EXIT_FAILURE);
	}

	std::string line;
	while (std::getline(std::cin, line))
	{
		if (line == "exit")
		{
			break;
		}
		send(client_fd, line.c_str(), line.size(), 0);
	}

	close(client_fd);

	return 0;

}