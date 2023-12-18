

#pragma once
#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "SyntaxValidator.hpp"
#include "TreeBuilder.hpp"
#include "LogicValidator.hpp"

#include <set>
#include <fstream>



class ConfigParser
{
private:
	std::string					configFileName;
	std::string					configFileContent;
	std::vector<std::string>	configTokens;
	ConfigNode					*configTreeRoot;
	LogicValidator				logicValidator;

	ConfigParser();

public:
	ConfigParser(const std::string &fileName);

	void	readConfigFile();
	void	tokenizeConfigFile();
	void	parseConfigFile();

	// Getters
	ConfigNode					*getConfigTreeRoot();
	std::vector<std::string>	&getConfigTokens();

};






#endif // CONFIGPARSER_HPP