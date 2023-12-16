#include "ConfigNode.hpp"
#include "DirectiveNode.hpp"
#include "ContextNode.hpp"
#include "ConfigNode.hpp"
#include "ConfigTokenizer.hpp"
#include "SyntaxValidator.hpp"

#include <fstream>
#include <iostream>

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

		ConfigTokenizer::tokenize(input, tokens);

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