# include "logicValidate.hpp"


LogicValidate::LogicValidate()
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


void    LogicValidate::validateDirectives(const ConfigNode *node)
{
	std::vector<std::string> tmp;
	std::map<std::string, std::pair<int , int> >  possibleDirs;
	// possibleDirs["root"] = std::make_pair(OneArg, Independent); /*only one*/
	// possibleDirs["listen"] = std::make_pair(OneArg, ParentNeeded); /*only one*/
	// possibleDirs["autoindex"] = std::make_pair(OneArg, Independent); /*only one*/
	// possibleDirs["server_name"] = std::make_pair(OneArg, ParentNeeded); /*only one*/
	// possibleDirs["client_max_body_size"] = std::make_pair(OneArg, Independent); /*only one*/
	// possibleDirs["error_page"] = std::make_pair(TwoArgs, Independent); /*two or more*/
	// possibleDirs["try_files"] = std::make_pair(TwoOrMoreArgs, ParentNeeded); /*two or more*/
	// possibleDirs["rewrite"] = std::make_pair(TwoArgs, ParentNeeded); /* only 2*/
	// possibleDirs["index"] = std::make_pair(OneOrMoreArgs, Independent); /*one or more*/
	// possibleDirs["return"] = std::make_pair(OneOrTwoArgs, ParentNeeded); /*one or two*/

	if (node->getType() == Context)
	{
		ContextNode *contextNode = (ContextNode *)(node);
		for (int i = 0; i < contextNode->getNumChildren(); i++)
			validateDirectives(contextNode->getChildren()[i]);
	}
	if (node->getType() == Directive)
	{
		DirectiveNode *directive = (DirectiveNode *)(node);
		std::map<std::string, std::pair<int, int> >::iterator it = possibleDirs.find(directive->getKey());
		if (it != possibleDirs.end())
		{
			 ContextNode *parentNode = ( ContextNode *)(directive->getParent());
			
			if (it->second.second == ParentNeeded) // validate parent
				validateDirectiveParent(it->first, parentNode->getName());


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

			validateDirectiveCodes(directive);
		}
		else {
			throw (std::runtime_error("Unknown directive \"" + directive->getKey() + "\""));
		
		}
	}
}

void	LogicValidate::validateDirectiveCodes(DirectiveNode *directiveNode)
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

void	LogicValidate::validateDirectiveParent(const std::string &key, const std::string &parentName)
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

LogicValidate::~LogicValidate()
{
    //load values 
}