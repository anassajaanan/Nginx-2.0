

#pragma once
#include "ConfigNode.hpp"
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
	static ConfigNode	*buildTree(ConfigNode *parent, std::vector<std::string>::iterator &it,
									std::vector<std::string>::iterator &end);

public:
	

	static ConfigNode	*parseConfigToTree(std::vector<std::string> &tokens);
};


#endif // TREEBUILDER_HPP
