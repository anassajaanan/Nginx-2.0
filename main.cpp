#include "ConfigNode.hpp"
#include "DirectiveNode.hpp"
#include "ContextNode.hpp"
#include <cstddef>
#include <regex>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <set>


ConfigNode	*getDirectiveNode(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	DirectiveNode	*node = new DirectiveNode(*it, parent);
	it++;
	while (*it != ";")
		node->addValue(*it++);
	it++;
	return (node);
}

ConfigNode	*getContextNode(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	ConfigNode	*node = new ContextNode(*it, parent);
	it += 2;
	return (node);
}

ConfigNode	*getLocationNode(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	ConfigNode	*node = new ContextNode(*it, parent, *(it + 1));
	it += 3;
	return (node);
}




ConfigNode	*parse(ConfigNode *parent, std::vector<std::string>::iterator &it, std::vector<std::string>::iterator &end)
{
	if (it == end)
		return (NULL);
	if (*it == "}")
		return (parent);

	ConfigNode	*node = NULL;

	if (*it == "http" || *it == "server" || *it == "location")
	{
		if (*it == "location")
			node = getLocationNode(parent, it);
		else
			node = getContextNode(parent, it);
		while (*it != "}")
		{
			parse(node, it, end);
			if (it == end)
				break;
		}
		it++;
	}
	else
	{
		node = getDirectiveNode(parent, it);
	}

	if (parent && parent->getType() == Context)
	{
		static_cast<ContextNode *>(parent)->addChild(node);
	}

	return (node);
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

        tokenize(input, tokens);

        for (int i = 0; i < (int)tokens.size(); i++)
        {
            std::cout << "[" << tokens[i] << "]" << std::endl;
        }

        it = tokens.begin();
        std::vector<std::string>::iterator end = tokens.end();
        ConfigNode *tree = parse(NULL, it, end);
        return (0);
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }


}