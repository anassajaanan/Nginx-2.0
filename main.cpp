#include "ConfigNode.hpp"
#include "DirectiveNode.hpp"
#include "ContextNode.hpp"
#include "ConfigNode.hpp"
#include "parseException.hpp"
#include <regex>
#include <map>
#include <algorithm>
#include <stack>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


void		semicolon(std::vector<std::string> content)
{
	std::vector<std::string> data;
	data.push_back("http");
	data.push_back("server");
	data.push_back("location");
	data.push_back("{");
	data.push_back("}");
	std::vector<std::string>::iterator	it = content.begin();
	std::vector<std::string>::iterator	value;

	while (it != content.end())
	{
		value = std::find(data.begin(), data.end(), *it);
		std::cout << "it = " << *it << std::endl;
		if (value == data.end())
		{
			it++;
			if (it == content.end())
				throw (Semicolon());
			std::string::difference_type n = std::count(it->begin(), it->end(), ';');
			std::cout << "n = " << n << std::endl;
			if (n != 1)
				throw (Semicolon());
		}
		it++;
	}
}

// using similar concept of RPN
void		syntaxValidation(std::vector<std::string> content)
{
	std::stack<std::string>	st;
	std::string				saveKey;
	std::vector<std::string>::iterator	it = content.begin();
	std::vector<std::string>::iterator	tmp = it;

	if (content.empty())
		throw (EmptyConf());
	for(; it != content.end(); it++)
	{
		tmp = it;
		if (*it == "{")
		{
			saveKey = *--tmp;
			tmp++;
			st.push(*it);
			if (++tmp == content.end())
				throw (ExtraOpenBraces());
		}
		if ((saveKey.find_first_not_of("abcdefghijklmnopqrstuvwxyz/") != std::string::npos && *tmp == "{}")
		|| (*tmp == "{" && *++tmp == "}" && saveKey.find_first_not_of("abcdefghijklmnopqrstuvwxyz/") != std::string::npos)
		|| *tmp == "{}")
			throw (EmptyBraces());
		if (*it == "}")
		{
			if (st.empty()) /* when we got '}' put no '{' inside the st  */
				throw (ExtraCloseBraces());
			st.pop();
		}
	}
	if (st.size() > 0)
		throw (ExtraOpenBraces());
}

int main()
{

	try
	{
		std::ifstream	ifs("nginx.conf");
		std::vector<std::string>	serverVector;
		std::vector<std::string>::iterator it;
		// ConfigNode		head();
		// ConfigNode			*tmp;
		// ContextNode			*t;

		if (!ifs.is_open())
		{
			std::cout << "Could Not Open the file" << std::endl;
			return (1);
		}
		// std::string content;
		std::string line;
		while(std::getline(ifs, line))
		{
			std::istringstream iss(line);

			// loading inside the vector 
			for(std::string str; iss >> str;)
			{
				std::cout << "[" << str << "]" << std::endl;
				serverVector.push_back(str);
			}
		}
		ifs.close();
		syntaxValidation(serverVector);
		semicolon(serverVector);

	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return (0);


}