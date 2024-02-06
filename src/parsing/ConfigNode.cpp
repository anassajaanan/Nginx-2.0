#include "ConfigNode.hpp"


ConfigNode::ConfigNode(NodeType type, ConfigNode *parentNode)
	: type(type), parent(parentNode) { }


ConfigNode::~ConfigNode() { }


NodeType	ConfigNode::getType() const
{
	return (this->type);
}

ConfigNode	*ConfigNode::getParent()
{
	return (this->parent);
}
