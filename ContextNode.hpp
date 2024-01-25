

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
	int							numChildren;

public:
	ContextNode();
	ContextNode(const std::string &name, ConfigNode *parentNode = NULL, const std::string &path = "No Path Specified");
	~ContextNode();

	// getters
	const std::string				&getName() const;
	const std::string				&getPath() const;
	const std::vector<ConfigNode *>	&getChildren() const;
	int								getNumChildren() const;


	void	addChild(ConfigNode *child);

	int		getCountOf(const std::string &key) const;



	// TODO:
	/*
		Check for Duplicate Children: Depending on your requirements,
		you might want to check if a similar child already exists before adding a new one.

		Removing a Child: Implement a HttpRequest to remove a child, either by reference or by some identifier.

		Finding a Child: A HttpRequest to find a child based on some criteria (like name or type) could be helpful.

		Clearing All Children: A HttpRequest to remove all children, useful for cleanup or re-initialization.
	*/

};

#endif /* CONTEXTNODE_HPP */
