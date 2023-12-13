#include "ConfigNode.hpp"




class DirectiveNode : public ConfigNode
{
private:
	std::string	key;
	std::string	value;


public:
	DirectiveNode();
	DirectiveNode(const std::string &key, const std::string &value, ConfigNode *parentNode = nullptr);
	~DirectiveNode();


	// getters
	const std::string	&getKey() const;
	const std::string	&getValue() const;

};