# include "LogicValidator.hpp"
#include "ConfigNode.hpp"
#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include <stdexcept>


LogicValidator::LogicValidator()
{
    possibleDirs["root"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["listen"] = std::make_pair(OneArg, ParentNeeded); /*only one*/
	possibleDirs["autoindex"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["server_name"] = std::make_pair(OneArg, ParentNeeded); /*only one*/
	possibleDirs["client_max_body_size"] = std::make_pair(OneArg, Independent); /*only one*/
	possibleDirs["error_page"] = std::make_pair(TwoArgs, Independent); /*two or more*/
	possibleDirs["try_files"] = std::make_pair(TwoOrMoreArgs, ParentNeeded); /*two or more*/
	possibleDirs["rewrite"] = std::make_pair(TwoArgs, ParentNeeded); /* only 2*/
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


void	 LogicValidator::validateDirectiveDublicates(ConfigNode *node)
{
	if (node->getType() == Directive)
		return;
	// std::set<std::string> nonDublicatesDirectives;
	ContextNode *parent = static_cast<ContextNode *>(node);
	// const std::vector<ConfigNode *>children = parent->getChildren();
	for (size_t i = 0; i < parent->getChildren().size(); i++)
	{
		if (parent->getChildren()[i]->getType() == Directive)
		{
			DirectiveNode *directiveNode = static_cast<DirectiveNode *>(parent->getChildren()[i]);
			if (directiveNode->getKey() == "root" || directiveNode->getKey() == "client_max_body_size"
			|| directiveNode->getKey() == "try_files" || directiveNode->getKey() == "autoindex")
				if (parent->getCountOf(directiveNode->getKey()) > 1)
					throw (std::runtime_error("\"" + directiveNode->getKey() + "\"" + " directive is duplicated"));
		}
			validateDirectiveDublicates(parent->getChildren()[i]);
	}
}

void    LogicValidator::validate(ConfigNode *node)
{
	if (node->getType() == Context)
	{
		ContextNode *contextNode = static_cast<ContextNode *>(node);
		const std::vector<ConfigNode *> &children = contextNode->getChildren();
		for (int i = 0; i < contextNode->getNumChildren(); i++)
			validate(children[i]);
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
			validateDirectiveCodes(directive);
		}
		else
			throw (std::runtime_error("Unknown directive \"" + directive->getKey() + "\""));
	}
}

void	LogicValidator::validateDirectiveCodes(DirectiveNode *directiveNode)
{
	if (directiveNode->getKey() == "return")
	{
		try {
			int code = std::stoi(directiveNode->getValues()[0]);
			if (code < 100 || code > 599)
				throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"return\" directive"));
		}
		catch (std::exception &e) {
			throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"return\" directive"));
		}
	}
	else if (directiveNode->getKey() == "error_page")
	{
		try {
			int code = std::stoi(directiveNode->getValues()[0]);
			if (code < 300 || code > 599)
				throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"error_page\" directive"));
		}
		catch (std::exception &e) {
			throw (std::runtime_error("invalid return code " + directiveNode->getValues()[0] + " in \"error_page\" directive"));
		}
	}
	
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
		else if (key == "rewrite")
		{
			if (parentName == "http")
				throw (std::runtime_error("\"rewrite\" directive is not allowed in this context"));
		}
}

LogicValidator::~LogicValidator()
{
}