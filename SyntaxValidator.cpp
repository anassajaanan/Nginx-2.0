#include "SyntaxValidator.hpp"

SyntaxError::SyntaxError(const std::string &errorMessage)
	: std::runtime_error(errorMessage) { }

void		SyntaxValidator::validateBraces(const std::vector<std::string> &tokens)
{
	std::stack<std::string>	st;
	std::vector<std::string>::const_iterator	it = tokens.begin();

	if (tokens.empty())
		throw (SyntaxError(EMPTY_CONFIG_FILE));
	
	for(; it != tokens.end(); it++)
	{
		if (*it == "{")
		{
			if (*(it - 1) == "{")
				throw (SyntaxError(UNEXPECTED_OPEN_BRACE));
			if (*(it - 1) == "}" && (*(it - 2)).find_first_not_of("server") == std::string::npos)
				throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
			if (*(it - 1) == "}")
				throw (SyntaxError(UNEXPECTED_OPEN_BRACE));
			st.push(*it);
			if ((it + 1) == tokens.end())
				throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
		}
		if (*it == "}")
		{
			if (st.empty()) /* when we got '}' put no '{' inside the st  */
				throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
			st.pop();
		}
	}
	if (st.size() > 0)
		throw (SyntaxError(UNEXPECTED_OPEN_BRACE));
}

void	SyntaxValidator::validateRequiredContexts(const std::vector<std::string> &tokens)
{

	if (std::find(tokens.begin(), tokens.end(), "http") == tokens.end())
		throw (SyntaxError(MISSING_HTTP_CONTEXT));
	if (std::find(tokens.begin(), tokens.end(), "server") == tokens.end())
		throw (SyntaxError(MISSING_SERVER_CONTEXT));

	for (size_t i = 0; i < tokens.size(); ++i)
	{
        if (tokens[i] == "http" || tokens[i] == "server")
		{
			if ((i + 1) < tokens.size() && tokens[i + 1] == "}")
				throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
			if ((i + 1) < tokens.size() && tokens[i + 1] != "{")
				throw (SyntaxError(tokens[i] == "http" ? HTTP_CONTEXT_ERROR : SERVER_CONTEXT_ERROR));
        }
		if (tokens[i] == "location")
		{
			if ((i + 1) < tokens.size() && tokens[i + 1] == "}")
				throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
			if ((i + 1) < tokens.size() && tokens[i + 1] == "{")
				throw (SyntaxError(LOCATION_CONTEXT_ERROR));
			if ((i + 2) < tokens.size() && tokens[i + 2] != "{")
				throw (SyntaxError(LOCATION_CONTEXT_ERROR));
		}
    }
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
			if (!contextStack.empty())
				throw (SyntaxError(HTTP_CONTEXT_OUTSIDE_MAIN));
			contextStack.push("http");
			httpContextFound = true;
		}
		else if (tokens[i] == "server")
		{
			if (contextStack.empty() || contextStack.top() != "http")
				throw (SyntaxError(SERVER_CONTEXT_OUTSIDE_HTTP));
			contextStack.push("server");
			serverContextFound = true;
		}
		else if (tokens[i] == "location")
		{
			if (contextStack.empty() || contextStack.top() != "server")
				throw (SyntaxError(LOCATION_CONTEXT_OUTSIDE_SERVER));
			contextStack.push("location");
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