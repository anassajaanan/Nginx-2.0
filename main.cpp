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


void	logicValidation(ConfigNode *node)
{
	if (node->getType() == Context)
	{
		ContextNode	*contextNode = (ContextNode *)node;
		const std::vector<ConfigNode *>	&children = contextNode->getChildren();
		for (size_t i = 0; i < children.size(); i++)
		{
			logicValidation(children[i]);
		}
	}
	else if (node->getType() == Directive)
	{
		DirectiveNode *directiveNode = (DirectiveNode *)node;
		ContextNode	*parentNode = (ContextNode *)directiveNode->getParent();
		if (directiveNode->getKey() == "listen")
		{
			if (parentNode->getName() != "server")
				throw (std::runtime_error("\"listen\" directive is not allowed in this context"));
			if (directiveNode->getValueCount() != 1)
				throw (std::runtime_error("invalid number of arguments in \"listen\" directive"));
		}
		else if (directiveNode->getKey() == "server_name")
		{
			if (parentNode->getName() != "server")
				throw (std::runtime_error("\"server_name\" directive is not allowed in this context"));
			if (directiveNode->getValueCount() != 1)
				throw (std::runtime_error("invalid number of arguments in \"server_name\" directive"));
		}
		else if (directiveNode->getKey() == "try_files")
		{
			if (parentNode->getName() == "http")
				throw (std::runtime_error("\"try_files\" directive is not allowed in this context"));
			if (directiveNode->getValueCount() < 2)
				throw (std::runtime_error("invalid number of arguments in \"try_files\" directive"));
		}
		else if (directiveNode->getKey() == "return")
		{
			if (parentNode->getName() == "http")
				throw (std::runtime_error("\"return\" directive is not allowed in this context"));
			if (directiveNode->getValueCount() == 0 || directiveNode->getValueCount() > 2)
				throw (std::runtime_error("invalid number of arguments in \"return\" directive"));
			try {
				int code = std::stoi(directiveNode->getValues()[0]);
				if (code < 100 || code > 599)
					throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"return\" directive"));
			}
			catch (std::exception &e) {
				throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"return\" directive"));
			}
		}
		else if (directiveNode->getKey() == "rewrite")
		{
			// Todo : check if the regex is valid
		}
	}
}



int main()
{
    try
    {
        std::ifstream	ifs("nginx.conf");
        std::vector<std::string>	tokens;
        std::vector<std::string>::iterator	it;
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

        tokenize(input, tokens);

        for (int i = 0; i < (int)tokens.size(); i++)
        {
            std::cout << "[" << tokens[i] << "]" << std::endl;
        }

        it = tokens.begin();
        std::vector<std::string>::iterator end = tokens.end();
        
		
		ConfigNode *root = TreeBuilder::builder(NULL, it, end);


		return (0);

        return (0);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}