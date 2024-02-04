



#pragma once
#ifndef CONFIGNODE_HPP
#define CONFIGNODE_HPP

#include <string>
#include <iostream>
#include <vector>
#include <map>


enum NodeType
{
	Context,
	Directive
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
	ConfigNode			*getParent();

};


#endif /* CONFIGNODE_HPP */

