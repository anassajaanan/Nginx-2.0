
#ifndef PARSEEXCEPTION_HPP
#define PARSEEXCEPTION_HPP


#include "ConfigNode.hpp"
#include <exception>

#define MISSING_HTTP_CONTEXT 
#define MISSING_SERVER_CONTEXT 
#define HTTP_CONTEXT_OUTSIDE_MAIN 
#define SERVER_CONTEXT_OUTSIDE_HTTP 
#define LOCATION_CONTEXT_OUTSIDE_SERVER 
#define UNEXPECTED_CLOSING_BRACE 
#define MISSING_OPEN_BRACE_AFTER_HTTP 
#define MISSING_OPEN_BRACE_AFTER_SERVER 
#define MISSING_OPEN_BRACE_AFTER_LOCATION 
#define INVALID_LOCATION_FORMAT 
#define UNEXPECTED_SEMICOLONE 
#define MISSING_SEMICOLONE 
#define EMPTYCONF "Empty Configuration File"
#define ExtraOpenBraces "Unexpected \"{\""
#define ExtraCloseBraces "Unexpected \"}\""
#define EmptyBraces "Extra {} inside the configruation file"
#define WrongContextName "Wrong Context name"
#define HttpContextError "invalid number of arguments in \"http\" directive"
#define HttpOrderError "http should be first"
#define ServerContextError "invalid number of arguments in \"server\" directive"
#define LocationContextError "invalid number of arguments in \"location\" directive"


class SyntaxError : public std::runtime_error
{
    public:
        SyntaxError(const std::string &messageError);
};

#endif /* PARSEEXCEPTION_HPP */