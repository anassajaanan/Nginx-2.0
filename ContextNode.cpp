# include "ContextNode.hpp"

ContextNode::ContextNode(const std::string &name, ConfigNode *parentNode): ConfigNode(Context, parentNode)
{
}

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
}