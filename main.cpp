

// #include "ConfigParser.hpp"
#include "ServerConfig.hpp"
#include <cstddef>





int main()
{
    try {
		ServerConfig config;
		config.setListen("255.0.1.1:90");
		config.setAutoindex("on");
		config.setClientMaxBodySize("1008m");
		config.setErrorPage("300", "/index");
		std::cout << "Success!" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;

	
}