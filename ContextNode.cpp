# include "ContextNode.hpp"

ContextNode::ContextNode() : ConfigNode(Context) , name("DefaultContext") { }

ContextNode::ContextNode(const std::string &name, ConfigNode *parentNode) 
	: ConfigNode(Context, parentNode) , name(name) { }

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