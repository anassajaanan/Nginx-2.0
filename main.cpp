

#include "ConfigParser.hpp"



int main()
{
    try
    {
        ConfigParser	parser("nginx.conf");

		std::cout << "nginx 2.0 : the configuration file /etc/nginx/nginx.conf syntax is ok" << std::endl
				  << "nginx 2.0: configuration file /etc/nginx/nginx.conf test is successful" << std::endl;

		delete(parser.getConfigTreeRoot());

		return (0);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}