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
	if (!currentToken.empty())
		tokens.push_back(currentToken);
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

		SyntaxValidator::validate(tokens);
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return (0);


}