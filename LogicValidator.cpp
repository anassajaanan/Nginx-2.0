# include "LogicValidator.hpp"


LogicValidator::LogicValidator()
{
    possibleDirs["root"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["listen"] = std::make_pair(OneArg, ParentNeeded); /*only one*/
	possibleDirs["autoindex"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["server_name"] = std::make_pair(OneArg, ParentNeeded); /*only one*/
	possibleDirs["client_max_body_size"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["error_page"] = std::make_pair(TwoArgs, Independent); /*two or more*/
	possibleDirs["try_files"] = std::make_pair(TwoOrMoreArgs, ParentNeeded); /*two or more*/
	possibleDirs["index"] = std::make_pair(OneOrMoreArgs, Independent); /*one or more*/
	possibleDirs["return"] = std::make_pair(OneOrTwoArgs, ParentNeeded); /*one or two*/
}


void	LogicValidator::validateDirectiveArgs(DirectiveNode *directive, std::map<std::string, std::pair<int, int> >::iterator it)
{
	if (it->second.first == OneArg && directive->getValueCount() != 1)
		throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
	else if (it->second.first == TwoOrMoreArgs && directive->getValueCount() < 2)
		throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
	else if (it->second.first == OneOrMoreArgs && directive->getValueCount() < 1)
		throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
	else if (it->second.first == TwoArgs && directive->getValueCount() != 2)
		throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
	else if (it->second.first == OneOrTwoArgs && (directive->getValueCount() < 1 || directive->getValueCount() > 2))
		throw (std::runtime_error("Invalid number of arguments in \"" + directive->getKey() + "\" directive"));
}

void	LogicValidator::validateDirectiveParent(const std::string &key, const std::string &parentName)
{
		if (key == "listen")
		{
			if (parentName != "server")
				throw (std::runtime_error("\"listen\" directive is not allowed in this context"));
		}
		else if (key == "server_name")
		{
			if (parentName != "server")
				throw (std::runtime_error("\"server_name\" directive is not allowed in this context"));
		}
		else if (key == "try_files")
		{
			if (parentName == "http")
				throw (std::runtime_error("\"try_files\" directive is not allowed in this context"));
		}
		else if (key == "return")
		{
			if (parentName == "http")
				throw (std::runtime_error("\"return\" directive is not allowed in this context"));
		}
}

void	 LogicValidator::validateDirectiveDuplicates(ConfigNode *node)
{
	if (node->getType() == Directive)
		return;
	ContextNode *parent = static_cast<ContextNode *>(node);
	const std::vector<ConfigNode *>	&children = parent->getChildren();
	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i]->getType() == Directive)
		{
			DirectiveNode *directiveNode = static_cast<DirectiveNode *>(children[i]);
			const std::string	&key = directiveNode->getKey();
			if (key == "root" || key == "client_max_body_size"
			|| key == "try_files" || key == "autoindex")
				if (parent->getCountOf(key) > 1)
					throw (std::runtime_error("\"" + key + "\"" + " directive is duplicated"));
		}
		validateDirectiveDuplicates(children[i]);
	}
}

void    LogicValidator::validateConfigTree(ConfigNode *node)
{
	if (node->getType() == Context)
	{
		ContextNode *contextNode = static_cast<ContextNode *>(node);
		const std::vector<ConfigNode *> &children = contextNode->getChildren();
		for (int i = 0; i < contextNode->getNumChildren(); i++)
			validateConfigTree(children[i]);
	}
	if (node->getType() == Directive)
	{
		DirectiveNode *directive = static_cast<DirectiveNode *>(node);
		std::map<std::string, std::pair<int, int> >::iterator it = possibleDirs.find(directive->getKey());
		if (it != possibleDirs.end())
		{
			ContextNode *parentNode = static_cast<ContextNode *>(directive->getParent());
			if (it->second.second == ParentNeeded) // validate parent
				validateDirectiveParent(it->first, parentNode->getName());
			
			validateDirectiveArgs(directive, it);
		}
		else
			throw (std::runtime_error("Unknown directive \"" + directive->getKey() + "\""));
	}
}


void	LogicValidator::validate(ConfigNode *node)
{
	validateConfigTree(node);
	validateDirectiveDuplicates(node);
}


LogicValidator::~LogicValidator()
{
}