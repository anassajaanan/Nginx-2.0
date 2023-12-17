#include "ConfigNode.hpp"
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
#include "logicValidate.hpp"

void tokenize(const std::string &input, std::vector<std::string> &tokens)
{
    std::string					currentToken;
    std::set<char>				delimiters;
    delimiters.insert('{');
    delimiters.insert('}');
    delimiters.insert(';');

    for (std::string::const_iterator it = input.begin(); it != input.end(); it++)
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
        else
            currentToken += ch;
    }
    if (!currentToken.empty())
        tokens.push_back(currentToken);
}



int main()
{
    try
    {
        std::ifstream	ifs("nginx.conf");
        std::vector<std::string>	tokens;
        std::vector<std::string>::iterator	it;
        std::string							tmp;
		LogicValidate						logicValidation;

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

        tokenize(input, tokens);

        for (int i = 0; i < (int)tokens.size(); i++)
        {
            std::cout << "[" << tokens[i] << "]" << std::endl;
        }

        it = tokens.begin();
        std::vector<std::string>::iterator end = tokens.end();
        
		SyntaxValidator::validate(tokens);
		ConfigNode *root = TreeBuilder::builder(NULL, it, end);
		logicValidation.validateDirectives(root);

		std::cout << "nginx 2.0 : the configuration file /etc/nginx/nginx.conf syntax is ok" << std::endl
				  << "nginx 2.0: configuration file /etc/nginx/nginx.conf test is successful" << std::endl;
		return (0);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}