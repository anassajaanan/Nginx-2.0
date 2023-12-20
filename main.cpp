

// #include "ConfigParser.hpp"
#include "ServerConfig.hpp"





int main()
{
    try {
		ServerConfig config;
		config.setListen("255.0.1.1.:90");
		std::cout << "Success!" << std::endl;
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;

	
}