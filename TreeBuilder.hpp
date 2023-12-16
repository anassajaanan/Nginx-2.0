

#pragma once
#ifndef TREEBUILDER_HPP
#define TREEBUILDER_HPP

#include "DirectiveNode.hpp"
#include "ContextNode.hpp"


class TreeBuilder
{
private:
	static ConfigNode	*getDirectiveNode(ConfigNode *parent, std::vector<std::string>::iterator &it);
	static ConfigNode	*getContextNode(ConfigNode *parent, std::vector<std::string>::iterator &it);
	static ConfigNode	*getLocationNode(ConfigNode *parent, std::vector<std::string>::iterator &it);

public:
	static ConfigNode	*builder(ConfigNode *parent, std::vector<std::string>::iterator &it,
									std::vector<std::string>::iterator &end);
};


#endif // TREEBUILDER_HPP