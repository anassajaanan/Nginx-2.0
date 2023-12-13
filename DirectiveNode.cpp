#include "DirectiveNode.hpp"
#include "ConfigNode.hpp"
#include <string>

DirectiveNode::DirectiveNode() : ConfigNode(Directive)
{
	key = "DefaultKey";
	value = "DefaultValue";
}


DirectiveNode::DirectiveNode(const std::string &key, const std::string &value, ConfigNode *parentNode)
	: ConfigNode(Directive, parentNode), key(key), value(value) { }

DirectiveNode::~DirectiveNode() { }


const std::string	&DirectiveNode::getKey() const
{
	return (this->key);
}

const std::string	&DirectiveNode::getValue() const
{
	return (this->value);
}