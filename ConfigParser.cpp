#include "ConfigParser.hpp"


ConfigParser::ConfigParser(const std::string &fileName)
	: configFileName(fileName) { }

void	ConfigParser::readConfigFile()
{
	std::ifstream	file(configFileName.c_str());
	std::string		line;

	if (!file.is_open())
		throw std::runtime_error("Failed to open the config file");
	while (std::getline(file, line))
		configFileContent += line;
	file.close();
}

void	ConfigParser::tokenizeConfigFile()
{
	std::string					currentToken;
	std::set<char>				delimiters;
	delimiters.insert('{');
	delimiters.insert('}');
	delimiters.insert(';');

	for (std::string::iterator it = configFileContent.begin(); it != configFileContent.end(); it++)
	{
		char	ch = *it;
		if (delimiters.find(ch) != delimiters.end())
		{
			if (!currentToken.empty())
			{
				configTokens.push_back(currentToken);
				currentToken.clear();
			}
			configTokens.push_back(std::string(1, ch));
		}
		else if (std::isspace(ch))
		{
			if (!currentToken.empty())
			{
				configTokens.push_back(currentToken);
				currentToken.clear();
			}
		}
		else
			currentToken += ch;
	}
	if (!currentToken.empty())
		configTokens.push_back(currentToken);
}

void	ConfigParser::parseConfigFile()
{
	SyntaxValidator::validate(configTokens);
	configTreeRoot = TreeBuilder::parseConfigToTree(configTokens);
	logicValidator.validate(configTreeRoot);
}

ConfigNode	*ConfigParser::getConfigTreeRoot()
{
	return (configTreeRoot);
}

std::vector<std::string>	&ConfigParser::getConfigTokens()
{
	return (configTokens);
}