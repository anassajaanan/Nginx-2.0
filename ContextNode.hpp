

#pragma once
#include <string>
#ifndef CONTEXTNODE_HPP
#define CONTEXTNODE_HPP


#include "ConfigNode.hpp"

class ContextNode : public ConfigNode
{
private:
	std::string 				name;
	std::string 				path;
	std::vector<ConfigNode *>	children;

public:
	ContextNode();
	ContextNode(const std::string &name, ConfigNode *parentNode = NULL, const std::string &path = "No Path Specified");
	~ContextNode();

	// getters
	const std::string	&getName() const;
	std::vector<ConfigNode *>	&getChildren();


	void	addChild(ConfigNode *child);



	// TODO:
	/*
		Check for Duplicate Children: Depending on your requirements,
		you might want to check if a similar child already exists before adding a new one.

		Removing a Child: Implement a method to remove a child, either by reference or by some identifier.

		Finding a Child: A method to find a child based on some criteria (like name or type) could be helpful.

		Clearing All Children: A method to remove all children, useful for cleanup or re-initialization.
	*/

};

#endif /* CONTEXTNODE_HPP */