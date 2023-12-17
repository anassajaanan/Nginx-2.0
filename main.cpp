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


void	validateDirectiveParent(const std::string &key, const std::string &parentName)
{
		if (key == "listen")
		{
			if (parentName != "server")
				throw (std::runtime_error("\"listen\" directive is not allowed in this context"));
		}
		else if (key == "server_name")
		{
			if (parentName != "server")
				throw (std::runtime_error("\"server_name\" directive is not allowed in this context"));
		}
		else if (key == "try_files")
		{
			if (parentName == "http")
				throw (std::runtime_error("\"try_files\" directive is not allowed in this context"));
		}
		else if (key == "return")
		{
			if (parentName == "http")
				throw (std::runtime_error("\"return\" directive is not allowed in this context"));
		}
		else if (key == "rewrite")
		{
			if (parentName == "http")
				throw (std::runtime_error("\"rewrite\" directive is not allowed in this context"));
		}
}

void	validateDirectiveCodes(DirectiveNode *directiveNode)
{
	if (directiveNode->getKey() == "return")
	{
		try {
			int code = std::stoi(directiveNode->getValues()[0]);
			if (code < 100 || code > 599)
				throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"return\" directive"));
		}
		catch (std::exception &e) {
			throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"return\" directive"));
		}
	}
	else if (directiveNode->getKey() == "error_page")
	{
		try {
			int code = std::stoi(directiveNode->getValues()[0]);
			if (code < 300 || code > 599)
				throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"error_page\" directive"));
		}
		catch (std::exception &e) {
			throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"error_page\" directive"));
		}
	}
	
}

void    validateDirectives(const ConfigNode *node)
{
	std::vector<std::string> tmp;
	std::map<std::string, std::pair<int , int> >  possibleDirs;
	possibleDirs["root"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["listen"] = std::make_pair(OneArg, ParentNeeded); /*only one*/
	possibleDirs["autoindex"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["server_name"] = std::make_pair(OneArg, ParentNeeded); /*only one*/
	possibleDirs["client_max_body_size"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["error_page"] = std::make_pair(TwoArgs, Independent); /*two or more*/
	possibleDirs["try_files"] = std::make_pair(TwoOrMoreArgs, ParentNeeded); /*two or more*/
	possibleDirs["rewrite"] = std::make_pair(TwoArgs, ParentNeeded); /* only 2*/
	possibleDirs["index"] = std::make_pair(OneOrMoreArgs, Independent); /*one or more*/
	possibleDirs["return"] = std::make_pair(OneOrTwoArgs, ParentNeeded); /*one or two*/

	if (node->getType() == Context)
	{
		ContextNode *contextNode = (ContextNode *)(node);
		for (int i = 0; i < contextNode->getNumChildren(); i++)
			validateDirectives(contextNode->getChildren()[i]);
	}
	if (node->getType() == Directive)
	{
		DirectiveNode *directive = (DirectiveNode *)(node);
		std::map<std::string, std::pair<int, int> >::iterator it = possibleDirs.find(directive->getKey());
		if (it != possibleDirs.end())
		{
			 ContextNode *parentNode = ( ContextNode *)(directive->getParent());
			
			if (it->second.second == ParentNeeded) // validate parent
				validateDirectiveParent(it->first, parentNode->getName());


			if (it->second.first == OneArg && directive->getValueCount() != 1)
				throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
			else if (it->second.first == TwoOrMoreArgs && directive->getValueCount() < 2)
				throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
			else if (it->second.first == OneOrMoreArgs && directive->getValueCount() < 1)
				throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
			else if (it->second.first == TwoArgs && directive->getValueCount() != 2)
				throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
			else if (it->second.first == OneOrTwoArgs && (directive->getValueCount() < 1 || directive->getValueCount() > 2))
				throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));

			validateDirectiveCodes(directive);
		}
		else {
			throw (std::runtime_error("Unknown directive \"" + directive->getKey() + "\""));
		
		}
	}
}

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

		// logicValidation(root);
		validateDirectives(root);

		std::cout << "Success" << std::endl;
		return (0);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}