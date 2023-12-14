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
	data.push_back("listen");
	data.push_back("try_files");
	data.push_back("server_name");
	// data.push_back("");
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
			--tmp;
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
		std::string							tmp;
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
		std::string buffer;
		while(std::getline(ifs, line))
		{
			buffer += line;
		}
		std::cout << "buf = " << buffer << std::endl;
		std::cout << "len = " << buffer.length() << std::endl;
		ifs.close();
		std::string::iterator  buffer_it = buffer.begin();
		for (;buffer_it != buffer.end();buffer_it++)
		{
			if (*buffer_it == '{')
			{
				buffer.insert(buffer_it - buffer.begin(), "\n");
				buffer.insert(buffer_it - buffer.begin() + 2, "\n");
				buffer_it += 3;
			}
			if (*buffer_it == '}')
			{
				buffer.insert(buffer_it - buffer.begin(), "\n");
				buffer.insert(buffer_it - buffer.begin() + 2, "\n");
				buffer_it += 3;
			}
			if (*buffer_it == ';')
			{
				buffer.insert(buffer_it - buffer.begin(), "\n");
				buffer.insert(buffer_it - buffer.begin() + 2, "\n");
				buffer_it += 3;
			}
		}
		std::cout << "buf end = " << buffer << std::endl;

		it = serverVector.begin();
		// for (;it != serverVector.end(); it++)
		// {
		// 	std::cout << "[" << *it << "]" << std::endl;
		// }
		syntaxValidation(serverVector);
		semicolon(serverVector);

	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return (0);


}