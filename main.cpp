#include "ConfigNode.hpp"
#include "ConfigParser.hpp"
#include "DirectiveNode.hpp"
#include "ContextNode.hpp"
#include <cstddef>
#include <regex>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/_types/_size_t.h>
#include <vector>
#include <map>
#include <stack>
#include <set>


#include "TreeBuilder.hpp"
#include "SyntaxValidator.hpp"
#include "LogicValidator.hpp"

#include "ConfigParser.hpp"



int main()
{
    try
    {
        ConfigParser	parser("nginx.conf");

		std::cout << "nginx 2.0 : the configuration file /etc/nginx/nginx.conf syntax is ok" << std::endl
				  << "nginx 2.0: configuration file /etc/nginx/nginx.conf test is successful" << std::endl;
		return (0);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}