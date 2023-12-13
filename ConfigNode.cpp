#include "ConfigNode.hpp"


ConfigNode::ConfigNode(NodeType type, ConfigNode *parentNode)
	: type(type), parent(parentNode) { }


ConfigNode::~ConfigNode() { }


NodeType	ConfigNode::getType() const
{
	return (this->type);
}

const ConfigNode	*ConfigNode::getParent() const
{
	return (this->parent);
}