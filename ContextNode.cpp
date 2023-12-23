# include "ContextNode.hpp"
#include "ConfigNode.hpp"
#include "DirectiveNode.hpp"

ContextNode::ContextNode() : ConfigNode(Context) , name("DefaultContext"), path("No Path Specified"), numChildren(0) { }

ContextNode::ContextNode(const std::string &name, ConfigNode *parentNode, const std::string &path) 
	: ConfigNode(Context, parentNode) , name(name), path(path), numChildren(0) { }

const std::string	&ContextNode::getName() const
{
	return (this->name);
}

const std::string	&ContextNode::getPath() const
{
	return (this->path);
}

const std::vector<ConfigNode *>	&ContextNode::getChildren() const
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
	numChildren++;
}

int		ContextNode::getNumChildren() const
{
	return (this->numChildren);
}

int	ContextNode::getCountOf(const std::string &key) const
{
	int	count = 0;
	for (int i = 0; i < numChildren; i++)
	{
		if (children[i]->getType() == Directive)
		{
			DirectiveNode *directiveNode = static_cast<DirectiveNode *>(children[i]);
			if (directiveNode->getKey() == key)
				count++;
		}
	}
	return (count);
}