#include "ConfigTokenizer.hpp"

void	ConfigTokenizer::tokenize(const std::string &input, std::vector<std::string> &tokens)
{
	std::string					currentToken;
	std::set<char>				delimiters = {'{', '}', ';'};

	for (std::string::const_iterator it = input.begin(); it != input.end(); it++)
	{
		char	ch = *it;
		if (delimiters.find(ch) != delimiters.end())
		{
			if (!currentToken.empty())
			{
				tokens.push_back(currentToken);
				currentToken.clear();
			}
			tokens.push_back(std::string(1, ch));
		}
		else if (std::isspace(ch))
		{
			if (!currentToken.empty())
			{
				tokens.push_back(currentToken);
				currentToken.clear();
			}
		}
		else
		{
			currentToken += ch;
		}
	}
	if (!currentToken.empty())
		tokens.push_back(currentToken);
}
