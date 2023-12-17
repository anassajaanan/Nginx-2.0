
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
        std::map<std::string, int>  directiveContent;
    public:
        LogicValidate();
        ~LogicValidate();
};


#endif /* LOGICVALIDATE_HPP */