
#ifndef LOGICVALIDATE_HPP
#define LOGICVALIDATE_HPP

#include <string>
#include "ConfigNode.hpp"
#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include <map>
#include <iostream>

enum ParentPresence
{
    ParentNeeded,
    Independent
};

enum numArgs
{
    OneArg,
    TwoArgs,
	OneOrMoreArgs,
	TwoOrMoreArgs,
	OneOrTwoArgs
};


class   LogicValidate 
{
    private:
        std::map<std::string, std::pair<int , int> >  possibleDirs;
    public:
        LogicValidate();
        void    validateDirectives(const ConfigNode *node);
        void	validateDirectiveCodes(DirectiveNode *directiveNode);
        void	validateDirectiveParent(const std::string &key, const std::string &parentName);
        ~LogicValidate();
};


#endif /* LOGICVALIDATE_HPP */