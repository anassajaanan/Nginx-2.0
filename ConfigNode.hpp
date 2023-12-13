



#pragma once
#ifndef CONFIGNODE_HPP
#define CONFIGNODE_HPP

#include <string>


enum NodeType
{
	Context,
	Directive,
};

class ConfigNode
{
private:
	int			type;
	ConfigNode	*parent;

public:
	ConfigNode(int type, ConfigNode *parentNode = NULL);
	virtual ~ConfigNode();

	// getters
	int					getType() const;
	const ConfigNode	*getParent() const;

};


#endif /* CONFIGNODE_HPP */