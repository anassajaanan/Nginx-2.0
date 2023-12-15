#include "ConfigNode.hpp"
#include "DirectiveNode.hpp"
#include "ContextNode.hpp"
#include "ConfigNode.hpp"
#include "parseException.hpp"
#include <cctype>
#include <regex>
#include <map>
#include <algorithm>
#include <set>
#include <stack>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


// void	valdiateContexts(std::vector<std::string> content)
// {
// 	std::vector<std::string> data;
// 	data.push_back("server");
// 	data.push_back("http");

// 	std::vector<std::string>::iterator	it = data.begin();
// 	std::vector<std::string>::iterator	value;

// 	while (it != data.end())
// 	{
// 		value = std::find(content.begin(), content.end(), *it);
// 		// std::cout << "it = " << *it << std::endl;
// 		if (value == content.end())
// 		{
// 			throw (WrongContextName());
// 		}
// 		it++;
// 	}  
// }

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


std::vector<std::string>	tokenize(std::string &input)
{
	std::vector<std::string>	tokens;
	std::string					currentToken;
	// std::set<char>				delimiters {'{', '}', ';'};
	std::set<char>				delimiters;

	delimiters.insert('{');
	delimiters.insert('}');
	delimiters.insert(';');

	for (std::string::iterator it = input.begin(); it != input.end(); it++)
	{
		char	ch = *it;
		if (delimiters.find(ch) != delimiters.end())
		{
			if (!currentToken.empty())
			{
				tokens.push_back(currentToken);
				currentToken.clear();
			}
			tokens.push_back(std::string(1, ch));
		}
		else if (std::isspace(ch))
		{
			if (!currentToken.empty())
			{
				tokens.push_back(currentToken);
				currentToken.clear();
			}
		}
		else {
			currentToken += ch;
		}
	}
	return (tokens);
}


int main()
{

	try
	{
		std::ifstream	ifs("nginx.conf");
		std::vector<std::string>	tokens;
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
		std::string input;
		std::string line;
		std::string buffer;
		while(std::getline(ifs, line))
		{
			input += line;
		}
		ifs.close();

		tokens = tokenize(input);

		for (int i = 0; i < (int)tokens.size(); i++)
		{
			std::cout << "[" << tokens[i] << "]" << std::endl;
		}



		syntaxValidation(tokens);
		// valdiateContexts(tokens);
		// semicolon(serverVector);

	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return (0);


}