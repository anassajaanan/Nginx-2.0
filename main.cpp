#include "DirectiveNode.hpp"
#include "ContextNode.hpp"
#include <regex>
#include <sstream>
#include <fstream>
#include <iostream>




int main()
{
	std::ifstream	ifs("nginx.conf");

	if (!ifs.is_open())
	{
		std::cout << "Could Not Open the file" << std::endl;
		return (1);
	}
	std::string content;
	std::string line;
	while(std::getline(ifs, line))
	{
		content += line;
	}
	ifs.close();

	std::istringstream iss(content);

	// get tokens : split by \t\n\r\v\f
	std::vector<std::string> tokens;
	
	return (0);
}