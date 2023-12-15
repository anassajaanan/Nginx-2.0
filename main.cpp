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


void	valdiateContexts(std::vector<std::string> content)
{
	std::vector<std::string> data;
	data.push_back("server");
	data.push_back("http");

	std::vector<std::string>::iterator	it = data.begin();
	std::vector<std::string>::iterator	tmp = data.begin();
	std::vector<std::string>::iterator	value;
	int i = 0;

	while (it != data.end())
	{
		value = std::find(content.begin(), content.end(), *it);
		// std::cout << "it = " << *it << std::endl;
		if (value == content.end())
		{
			throw (WrongContextName());
		}
		it++;
	}
	it = content.begin();
	tmp = it;
	while (it != content.end())
	{
		tmp = it;
		value = std::find(content.begin(), content.end(), *it);
		if (*value == "http")
		{
			if (*value != *content.begin())
				throw (HttpContextError());
			if (*++tmp != "{")
				throw (WrongContextName());
		}
		else if (*value == "server")
		{
			if (*++tmp != "{")
				throw (ServerContextError());
		}
		else if (*value == "location")
		{
			tmp = value;
			tmp++;
			while (tmp != content.end() && *tmp != "{")
			{
				tmp++;
				i++;
			}
			if (i != 1)
				throw (LocationContextError());
		}
		it++;
	}
	//check for another arg for locaion
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
			if (saveKey == "{" ||  saveKey == "}")
				throw (ExtraBraces());
			tmp++;
			st.push(*it);
			if (++tmp == content.end())
				throw (ExtraOpenBraces());
			--tmp;
		}
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
		valdiateContexts(tokens);
		// semicolon(serverVector);

	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return (0);


}