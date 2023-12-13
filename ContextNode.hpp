#include "ConfigNode.hpp"
#include <string>
#include <vector>

class ContextNode : public ConfigNode
{
private:
	std::string 				name;
	std::vector<ConfigNode *>	children;

public:
	ContextNode();
	ContextNode(const std::string &name, ConfigNode *parentNode = nullptr);
	~ContextNode();

	// getters
	const std::string	&getName() const;
	std::vector<ConfigNode *>	&getChildren() const;
};