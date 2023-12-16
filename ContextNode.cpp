# include "ContextNode.hpp"
#include "ConfigNode.hpp"

ContextNode::ContextNode() : ConfigNode(Context) , name("DefaultContext"), path("No Path Specified") { }

ContextNode::ContextNode(const std::string &name, ConfigNode *parentNode, const std::string &path) 
	: ConfigNode(Context, parentNode) , name(name), path(path) { }

const std::string	&ContextNode::getName() const
{
	return (this->name);
}

std::vector<ConfigNode *>	&ContextNode::getChildren()
{
	return (this->children);
}

ContextNode::~ContextNode()
{
	std::vector<ConfigNode *>::iterator it = children.begin();
	while (it != children.end())
	{
		delete (*it);
		it++;
	}
	children.clear();
}

void	ContextNode::addChild(ConfigNode *child)
{
	if (child)
		children.push_back(child);
}