

#ifndef SYNTAXVALIDATOR_HPP
#define SYNTAXVALIDATOR_HPP

#include <iostream>
#include <vector>
#include <exception>
#include <unordered_set>
#include <stack>

#define MISSING_HTTP_CONTEXT 				"Missing Http Context"
#define MISSING_SERVER_CONTEXT 				"Missing Server Context"
#define HTTP_CONTEXT_OUTSIDE_MAIN 			"Http context outside main"
#define SERVER_CONTEXT_OUTSIDE_HTTP 		"Server context outside http context"
#define LOCATION_CONTEXT_OUTSIDE_SERVER 	"Location context outside server"
#define UNEXPECTED_CLOSING_BRACE 			"Unexpected \"}\""
#define MISSING_OPEN_BRACE_AFTER_HTTP 		"Missing open brace after http"
#define MISSING_OPEN_BRACE_AFTER_SERVER 	"Missing open brace after server"
#define MISSING_OPEN_BRACE_AFTER_LOCATION	"Missing open brace after location"
#define INVALID_LOCATION_FORMAT 			"Invalid location format"
#define UNEXPECTED_SEMICOLONE 				"Unexpected \";\""
#define EMPTY_CONFIG_FILE 					"Empty Configuration File"
#define EXTRA_OPEN_BRACES 					"Unexpected \"{\""
#define EXTRA_CLOSE_BRACES 					"Unexpected \"}\""
#define EMPTY_BRACES 						"Extra {} inside the configruation file"
#define WRONG_CONTEXT_NAME 					"Wrong Context name"
#define HTTP_CONTEXT_ERROR 					"invalid number of arguments in \"http\" directive"
#define HTTP_ORDER_ERROR 					"http should be first"
#define SERVER_CONTEXT_ERROR 				"invalid number of arguments in \"server\" directive"
#define LOCATION_CONTEXT_ERROR 				"invalid number of arguments in \"location\" directive"


class SyntaxError : public std::runtime_error
{
    public:
	    SyntaxError(const std::string &errorMessage);
};



class SyntaxValidator
{

    private:

        static void validateBraces(const std::vector<std::string> &tokens);
        static void valdiateContextsSyntax(const std::vector<std::string> &tokens);
        static void	validateContextLoop(std::vector<std::string>::const_iterator &it,
            const std::vector<std::string> &tokens, const std::vector<std::string> &data);
        static void	validateHttpContext(const std::vector<std::string> &tokens,
            std::stack<std::string> &contextStack, int i);
        static void	validateServerContext(const std::vector<std::string> &tokens,
            std::stack<std::string> &contextStack, int i);
        static void	validateLocationContext(const std::vector<std::string> &tokens,
            std::stack<std::string> &contextStack, int i);
        static void validateContexts(const std::vector<std::string> &tokens);
        static void	validateDirectives(const std::vector<std::string> &tokens);


    public:
        static void	validate(const std::vector<std::string> &tokens);

};


#endif // SYNTAXVALIDATOR_HPP