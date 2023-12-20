

// #include "ConfigParser.hpp"
#include "ServerConfig.hpp"
#include <cstddef>





int main(int ac, char **av)
{
    try {
		(void)ac;
		ServerConfig config;
		config.setListen("255.0.1.1:90");
		config.setAutoindex("on");
		config.setClientMaxBodySize(av[1]);
		std::cout << "Success!" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;

	
}