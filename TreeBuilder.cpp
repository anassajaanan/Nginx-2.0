#include "TreeBuilder.hpp"
#include "ConfigNode.hpp"




ConfigNode	*TreeBuilder::getDirectiveNode(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	DirectiveNode	*node = new DirectiveNode(*it, parent);
	it++;
	while (*it != ";")
		node->addValue(*it++);
	it++;
	return (node);
}

ConfigNode	*TreeBuilder::getContextNode(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	ConfigNode	*node = new ContextNode(*it, parent);
	it += 2;
	return (node);
}

ConfigNode	*TreeBuilder::getLocationNode(ConfigNode *parent, std::vector<std::string>::iterator &it)
{
	ConfigNode	*node = new ContextNode(*it, parent, *(it + 1));
	it += 3;
	return (node);
}

ConfigNode	*TreeBuilder::buildTree(ConfigNode *parent, std::vector<std::string>::iterator &it, std::vector<std::string>::iterator &end)
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
			buildTree(node, it, end);
			if (it == end)
				break;
		}
		it++;
	}
	else
		node = getDirectiveNode(parent, it);

	if (parent && parent->getType() == Context)
		static_cast<ContextNode *>(parent)->addChild(node);

	return (node);
}

ConfigNode	*TreeBuilder::parseConfigToTree(std::vector<std::string> &tokens)
{
	std::vector<std::string>::iterator it = tokens.begin();
	std::vector<std::string>::iterator eit = tokens.end();
	return (buildTree(NULL, it, eit));
}