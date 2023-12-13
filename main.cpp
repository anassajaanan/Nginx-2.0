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


ConfigNode	*parse(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	if ((*it) == "}")
		return (parent);
	
	if (*(it + 1) == "{")
	{
		if (parent == NULL)
		{
			parent = getContextNode(parent, it);
		}
		else
		{
			ContextNode	*p = dynamic_cast<ContextNode *>(parent);
			parent = getContextNode(parent, it);
			p->getChildren().push_back(parent);
		}
	}
	else
	{
		ConfigNode *directiveNode = getDirectiveNode(parent, it);
		ContextNode	*p = dynamic_cast<ContextNode *>(parent);
		p->getChildren().push_back(directiveNode);
	}
	parse(parent, it);

	return (parent);
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
		content += line;
	}
	ifs.close();

	std::istringstream iss(content);

	std::string token;
	while(iss >> token)
	{
		tokens.push_back(token);
	}

	for (int i = 0; i < (int)tokens.size(); i++)
	{
		std::cout << "[" << tokens[i] << "]" << std::endl;
	}


	std::vector<std::string>::iterator it = tokens.begin();
	ConfigNode *node = parse(NULL, it);



	return (0);


}