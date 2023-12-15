
#include "syntaxValidation.hpp"
#include <sys/_types/_size_t.h>

SyntaxError::SyntaxError(const std::string &errorMessage)
	: std::runtime_error(errorMessage) { }

void	SyntaxValidator::validateBraces(const std::vector<std::string> &tokens)
{
	std::stack<std::string>	stack;
	std::vector<std::string>::const_iterator	it = tokens.begin();

	if (tokens.empty())
		throw (SyntaxError(EMPTY_CONFIG_FILE));
	while (it != tokens.end())
	{
		if (*it == "{")
			stack.push(*it);
		if (*it == "}")
		{
			if (stack.empty())
				throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
			stack.pop();
		}
		it++;
	}
	if (stack.size() > 0)
		throw (SyntaxError(UNEXPECTED_OPEN_BRACE));
}

void	SyntaxValidator::validateRequiredContexts(const std::vector<std::string> &tokens)
{

	if (std::find(tokens.begin(), tokens.end(), "http") == tokens.end())
		throw (SyntaxError(MISSING_HTTP_CONTEXT));
	if (std::find(tokens.begin(), tokens.end(), "server") == tokens.end())
		throw (SyntaxError(MISSING_SERVER_CONTEXT));

	for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "http" || tokens[i] == "server") {
            if (i + 1 >= tokens.size() || tokens[i + 1] != "{") {
                throw SyntaxError(tokens[i] == "http" ? HTTP_CONTEXT_ERROR : SERVER_CONTEXT_ERROR);
            }
        }
    }
}


void	SyntaxValidator::validateHttpContext(const std::vector<std::string> &tokens,
								std::stack<std::string> &contextStack, size_t i)
{
	if ((i + 1) < tokens.size() && tokens[i + 1] == "}")
		throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
	else if ((i + 1) < tokens.size() && tokens[i + 1] == "{")
	{
		if (!contextStack.empty())
			throw (SyntaxError(HTTP_CONTEXT_OUTSIDE_MAIN));
		contextStack.push("http");
	}
	else
		throw (SyntaxError(MISSING_OPEN_BRACE_AFTER_HTTP));
}

void	SyntaxValidator::validateServerContext(const std::vector<std::string> &tokens,
								std::stack<std::string> &contextStack, size_t i)
{
	if ((i + 1) < tokens.size() && tokens[i + 1] == "}")
		throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
	else if ((i + 1) < tokens.size() && tokens[i + 1] == "{")
	{
		if (contextStack.empty() || contextStack.top() != "http")
			throw (SyntaxError(SERVER_CONTEXT_OUTSIDE_HTTP));
		contextStack.push("server");
	}
	else
		throw (SyntaxError(MISSING_OPEN_BRACE_AFTER_SERVER));
}

void	SyntaxValidator::validateLocationContext(const std::vector<std::string> &tokens,
								std::stack<std::string> &contextStack, size_t i)
{
	if ((i + 1) < tokens.size() && tokens[i + 1] == "{")
		throw (SyntaxError(LOCATION_CONTEXT_ERROR));
	else if ((i + 1) < tokens.size() && tokens[i + 1] == "}")
		throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
	else if ((i + 2) < tokens.size() && tokens[i + 2] == "{")
	{
		if (contextStack.empty() || contextStack.top() != "server")
			throw (SyntaxError(LOCATION_CONTEXT_OUTSIDE_SERVER));
		contextStack.push("location");
	}
	else
		throw (SyntaxError(MISSING_OPEN_BRACE_AFTER_LOCATION));
}

void	SyntaxValidator::validateContexts(const std::vector<std::string> &tokens)
{
	std::stack<std::string> contextStack;
	bool	httpContextFound = false;
	bool	serverContextFound = false;

	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "http")
		{
			validateHttpContext(tokens, contextStack, i);
			httpContextFound = true;
		}
		else if (tokens[i] == "server")
		{
			validateServerContext(tokens, contextStack, i);
			serverContextFound = true;
		}
		else if (tokens[i] == "location")
		{
			validateLocationContext(tokens, contextStack, i);
		}
		else if (tokens[i] == "}")
		{
			if (contextStack.empty())
				throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
			contextStack.pop();
		}
	}
	if (!httpContextFound)
		throw (SyntaxError(MISSING_HTTP_CONTEXT));
	if (!serverContextFound)
		throw (SyntaxError(MISSING_SERVER_CONTEXT));
	if (!contextStack.empty())
		throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
}

void	SyntaxValidator::validateDirectives(const std::vector<std::string> &tokens)
{
	std::unordered_set<std::string> contextKeywords;

	contextKeywords.insert("http");
	contextKeywords.insert("server");
	contextKeywords.insert("location");
	contextKeywords.insert("{");
	contextKeywords.insert("}");

	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (contextKeywords.find(tokens[i]) != contextKeywords.end())
		{
			if (tokens[i] == "location")
				i++;
			continue;
		}
		if (tokens[i] == ";")
			throw (SyntaxError(UNEXPECTED_SEMICOLONE));
		while (i < tokens.size() && tokens[i] != ";" && contextKeywords.find(tokens[i]) == contextKeywords.end())
			i++;
		if (i == tokens.size() || contextKeywords.find(tokens[i]) != contextKeywords.end())
			throw (SyntaxError(MISSING_SEMICOLONE));
	}
}

void	SyntaxValidator::validate(const std::vector<std::string> &tokens)
{
    validateBraces(tokens);
    validateRequiredContexts(tokens);
    validateContexts(tokens);
    validateDirectives(tokens);
}