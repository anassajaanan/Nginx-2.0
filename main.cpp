

// #include "ConfigParser.hpp"
#include "ServerConfig.hpp"
#include <cstddef>





int main()
{
    try
	{
		ServerConfig	server;

		server.setClientMaxBodySize("100000000000G");

		std::cout << "Success" << std::endl;



	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;

	
}