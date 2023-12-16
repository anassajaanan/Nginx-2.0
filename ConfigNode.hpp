



#pragma once
#ifndef CONFIGNODE_HPP
#define CONFIGNODE_HPP

#include <string>
#include <vector>


enum NodeType
{
	Context,
	Directive,
};

class ConfigNode
{
private:
	NodeType	type;
	ConfigNode	*parent;

public:
	ConfigNode(NodeType type, ConfigNode *parentNode = NULL);
	virtual ~ConfigNode();

	// getters
	NodeType			getType() const;
	const ConfigNode	*getParent() const;

};


#endif /* CONFIGNODE_HPP */