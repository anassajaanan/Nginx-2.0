
#include "syntaxValidation.hpp"

SyntaxError::SyntaxError(const std::string &errorMessage)
	: std::runtime_error(errorMessage) { }


void	validateHttpContext(const std::vector<std::string> &tokens,
								std::stack<std::string> &contextStack, int i)
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
								std::stack<std::string> &contextStack, int i)
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
								std::stack<std::string> &contextStack, int i)
{
	if ((i + 1) < tokens.size() && tokens[i + 1] == "{")
		throw (SyntaxError(INVALID_LOCATION_FORMAT));
	else if ((i + 1) < tokens.size() && tokens[i + 1] == "}")
		throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
	else if ((i + 2) < tokens.size() && tokens[i + 2] == "{")
	{
		if (contextStack.empty() || contextStack.top() != "server")
			throw (SyntaxError(LOCATION_CONTEXT_OUTSIDE_SERVER));
		contextStack.push("location");
	}
	else if ((i + 2) < tokens.size() && tokens[i + 2] == "}")
		throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));
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

void SyntaxValidator::validateDirectives(const std::vector<std::string> &tokens)
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
			continue;
		if (tokens[i] == ";")
			throw (SyntaxError(UNEXPECTED_SEMICOLONE));
		while (i < tokens.size() && tokens[i] != ";" && contextKeywords.find(tokens[i]) == contextKeywords.end())
			i++;
		if (i == tokens.size() || contextKeywords.find(tokens[i]) != contextKeywords.end())
			throw (SyntaxError(UNEXPECTED_CLOSING_BRACE));

	}
}

void		SyntaxValidator::validateBraces(const std::vector<std::string> &tokens)
{
	std::stack<std::string>	st;
	std::string				saveKey;
	std::vector<std::string>::const_iterator	it = tokens.begin();
	std::vector<std::string>::const_iterator	tmp = it;

	if (tokens.empty())
		throw (SyntaxError(EMPTY_CONFIG_FILE));
	for(; it != tokens.end(); it++)
	{
		tmp = it;
		if (*it == "{")
		{
			saveKey = *--tmp;
			if (saveKey == "{")
				throw (SyntaxError(EXTRA_OPEN_BRACES));
			if (saveKey == "}")
				throw (SyntaxError(EXTRA_CLOSE_BRACES));
			tmp++;
			st.push(*it);
			if (++tmp == tokens.end())
				throw (SyntaxError(EXTRA_CLOSE_BRACES));
			--tmp;
		}
		if (*it == "}")
		{
			if (st.empty()) /* when we got '}' put no '{' inside the st  */
				throw (SyntaxError(EXTRA_CLOSE_BRACES));
			st.pop();
		}
	}
	if (st.size() > 0)
		throw (SyntaxError(EXTRA_OPEN_BRACES));
}

void	SyntaxValidator::validateContextLoop(std::vector<std::string>::const_iterator &it, const std::vector<std::string> &tokens, const std::vector<std::string> &data)
{
	std::vector<std::string>::const_iterator	tmp = data.begin();
	std::vector<std::string>::const_iterator	value;
	int									i = 0;

	while (it != tokens.end())
	{
		tmp = it;
		value = std::find(tokens.begin(), tokens.end(), *it);
		if (*value == "http")
		{
			if (*value != *tokens.begin())
				throw (SyntaxError(HTTP_ORDER_ERROR));
			if (*++tmp != "{")
				throw (SyntaxError(HTTP_CONTEXT_ERROR));
		}
		else if (*value == "server")
		{
			if (*++tmp != "{")
				throw (SyntaxError(SERVER_CONTEXT_ERROR));
		}
		else if (*value == "location")
		{
			tmp = value;
			tmp++;
			while (tmp != tokens.end() && *tmp != "{")
			{
				tmp++;
				i++;
			}
			if (i != 1)
				throw (SyntaxError(LOCATION_CONTEXT_ERROR));
		}
		it++;
	}
}


void	SyntaxValidator::valdiateContextsSyntax(const std::vector<std::string> &tokens)
{
	std::vector<std::string> data;
	std::vector<std::string>::const_iterator	it = data.begin();
	std::vector<std::string>::const_iterator	value;

	data.push_back("server");
	data.push_back("http");
	while (it != data.end())
	{
		value = std::find(tokens.begin(), tokens.end(), *it);
		if (value == tokens.end())
		{
			throw (SyntaxError(WRONG_CONTEXT_NAME));
		}
		it++;
	}
	SyntaxValidator::validateContextLoop(it, tokens, data);
	//check for another arg for locaion
}

void	SyntaxValidator::validate(const std::vector<std::string> &tokens)
{
    validateBraces(tokens);
    valdiateContextsSyntax(tokens);
    validateContexts(tokens);
    validateDirectives(tokens);
}