#include "ConfigNode.hpp"


ConfigNode::ConfigNode(int type, ConfigNode *parentNode)
	: type(type), parent(parentNode) { }


ConfigNode::~ConfigNode() { }


int	ConfigNode::getType() const
{
	return (this->type);
}

const ConfigNode	*ConfigNode::getParent() const
{
	return (this->parent);
}