#include "ConfigParser.hpp"


ConfigParser::ConfigParser(const std::string &fileName)
	: configFileName(fileName), configTreeRoot(NULL) { }

ConfigParser::~ConfigParser()
{
	if (configTreeRoot)
		delete configTreeRoot;
}

void	ConfigParser::readConfigFile()
{
	struct stat		fileStat;
	if (stat(configFileName.c_str(), &fileStat) == 0)
	{
		if (!S_ISREG(fileStat.st_mode))
			throw std::runtime_error("Error: The specified configuration file ('"
			+ configFileName + "') is not a regular file. Please provide a valid file.");
	}
	else
		throw std::runtime_error("Error: Unable to find the configuration file ('"
		+ configFileName + "'). Please check the file path and try again.");

	std::ifstream	file(configFileName.c_str());
	std::string		line;

	if (!file.is_open())
		throw std::runtime_error("Error: Unable to open the configuration file ('"
		+ configFileName + "') for reading. Please check file permissions and try again.");
	while (std::getline(file, line))
		configFileContent += line + "\n";
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
		else if (ch == '"' && currentToken.empty())
		{
			it++;
			while (it != configFileContent.end() && *it != '"')
			{
				currentToken += *it;
				it++;
			}
			if (it == configFileContent.end())
				throw std::runtime_error("unexpected end of file, expecting \";\" or \"}\"");
			configTokens.push_back(currentToken);
			currentToken.clear();
		}
		else if (ch == '#')
		{
			if (!currentToken.empty())
			{
				configTokens.push_back(currentToken);
				currentToken.clear();
			}
			while (it != configFileContent.end() && *it != '\n')
				it++;
		}
		else
			currentToken += ch;
	}
	if (!currentToken.empty())
		configTokens.push_back(currentToken);
}

void	ConfigParser::parseConfigFile()
{
	readConfigFile();
	tokenizeConfigFile();

	SyntaxValidator::validate(configTokens);
	configTreeRoot = TreeBuilder::parseConfigToTree(configTokens);
	logicValidator.validate(configTreeRoot);
}



// Getters

ConfigNode	*ConfigParser::getConfigTreeRoot()
{
	return (configTreeRoot);
}

std::vector<std::string>	&ConfigParser::getConfigTokens()
{
	return (configTokens);
}