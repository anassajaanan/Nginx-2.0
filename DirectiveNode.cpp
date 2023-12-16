#include "DirectiveNode.hpp"
#include "ConfigNode.hpp"
#include <string>

DirectiveNode::DirectiveNode() : ConfigNode(Directive)
{
	key = "DefaultKey";
	valueCount = 0;
}


DirectiveNode::DirectiveNode(const std::string &key, ConfigNode *parentNode)
	: ConfigNode(Directive, parentNode), key(key) { }

DirectiveNode::~DirectiveNode()
{
	values.clear();
}


const std::string	&DirectiveNode::getKey() const
{
	return (this->key);
}

const std::vector<std::string>	&DirectiveNode::getValues() const
{
	return (this->values);
}

void	DirectiveNode::addValue(const std::string &value)
{
	this->values.push_back(value);
	this->valueCount++;
}