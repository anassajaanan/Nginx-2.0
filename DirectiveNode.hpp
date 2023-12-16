
#pragma once
#include <vector>
#ifndef DIRECTIVENODE_HPP
#define DIRECTIVENODE_HPP


#include "ConfigNode.hpp"




class DirectiveNode : public ConfigNode
{
private:
	std::string					key;
	std::vector<std::string>	values;
	int							valueCount;

public:
	DirectiveNode();
	DirectiveNode(const std::string &key, ConfigNode *parentNode = NULL);
	~DirectiveNode();


	// getters
	const std::string	&getKey() const;
	const std::vector<std::string>	&getValues() const;

	void	addValue(const std::string &value);

};

#endif /* DIRECTIVENODE_HPP */