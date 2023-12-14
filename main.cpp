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


ConfigNode	*getDirectiveNode(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	ConfigNode	*node = new DirectiveNode(*it, *(it + 1), parent);
	it += 2;
	return (node);
}

ConfigNode	*getContextNode(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	ConfigNode	*node = new ContextNode(*it, parent);
	it += 2;
	return (node);
}




ConfigNode	*parse(ConfigNode *parent, std::vector<std::string>::iterator &it, std::vector<std::string>::iterator &end)
{
	if (it == end)
		return (NULL);
	if (*it == "}")
		return (parent);

	ConfigNode	*node = NULL;

	if (*(it + 1) == "{")
	{
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

	if (parent && parent->getType() == NodeType::Context)
	{
		static_cast<ContextNode *>(parent)->addChild(node);
	}

	return (node);
}


int main()
{
	std::vector<std::string> tokens;
	std::ifstream	ifs("nginx.conf");

	if (!ifs.is_open())
	{
		std::cout << "Could Not Open the file" << std::endl;
		return (1);
	}
	std::string content;
	std::string line;
	while(std::getline(ifs, line))
	{
        std::istringstream iss(line);

        std::string token;
        while(iss >> token)
        {
            tokens.push_back(token);
        }
	}
	ifs.close();



	for (int i = 0; i < (int)tokens.size(); i++)
	{
		std::cout << "[" << tokens[i] << "]" << std::endl;
	}


	std::vector<std::string>::iterator it = tokens.begin();
	std::vector<std::string>::iterator end = tokens.end();
	ConfigNode *node = parse(NULL, it, end);



	return (0);


}