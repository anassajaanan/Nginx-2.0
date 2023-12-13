# include "ContextNode.hpp"
#include "ConfigNode.hpp"

ContextNode::ContextNode() : ConfigNode(NodeType::Context) , name("DefaultContext") { }

ContextNode::ContextNode(const std::string &name, ConfigNode *parentNode) 
	: ConfigNode(NodeType::Context, parentNode) , name(name) { }

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
}