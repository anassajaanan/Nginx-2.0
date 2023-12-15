#include "ConfigNode.hpp"
#include "DirectiveNode.hpp"
#include "ContextNode.hpp"
#include "ConfigNode.hpp"
#include "syntaxValidation.hpp"
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

// void	validateContextLoop(std::vector<std::string>::iterator &it, std::vector<std::string> content, std::vector<std::string> data)
// {
// 	std::vector<std::string>::iterator	tmp = data.begin();
// 	std::vector<std::string>::iterator	value;
// 	int									i = 0;

// 	while (it != content.end())
// 	{
// 		tmp = it;
// 		value = std::find(content.begin(), content.end(), *it);
// 		if (*value == "http")
// 		{
// 			if (*value != *content.begin())
// 				throw (SyntaxError(HTTP_ORDER_ERROR));
// 			if (*++tmp != "{")
// 				throw (SyntaxError(HTTP_CONTEXT_ERROR));
// 		}
// 		else if (*value == "server")
// 		{
// 			if (*++tmp != "{")
// 				throw (SyntaxError(SERVER_CONTEXT_ERROR));
// 		}
// 		else if (*value == "location")
// 		{
// 			tmp = value;
// 			tmp++;
// 			while (tmp != content.end() && *tmp != "{")
// 			{
// 				tmp++;
// 				i++;
// 			}
// 			if (i != 1)
// 				throw (SyntaxError(LOCATION_CONTEXT_ERROR));
// 		}
// 		it++;
// 	}
// }

// void	valdiateContextsSyntax(std::vector<std::string> content)
// {
// 	std::vector<std::string> data;
// 	std::vector<std::string>::iterator	it = data.begin();
// 	std::vector<std::string>::iterator	value;

// 	data.push_back("server");
// 	data.push_back("http");
// 	while (it != data.end())
// 	{
// 		value = std::find(content.begin(), content.end(), *it);
// 		if (value == content.end())
// 		{
// 			throw (SyntaxError(WRONG_CONTEXT_NAME));
// 		}
// 		it++;
// 	}
// 	validateContextLoop(it, content, data);
// 	//check for another arg for locaion
// }



// using similar concept of RPN



std::vector<std::string>	tokenize(std::string &input)
{
	std::vector<std::string>	tokens;
	std::string					currentToken;
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
		// SyntaxValidator s;

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

		SyntaxValidator::validate(tokens);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return (0);


}