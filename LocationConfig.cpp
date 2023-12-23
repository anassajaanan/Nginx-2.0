#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

LocationConfig::LocationConfig(const ServerConfig &serverConfig)
{
	this->root = serverConfig.root;
	this->index = serverConfig.index;
	this->autoindex = serverConfig.autoindex;
	this->errorPages = serverConfig.errorPages;
	this->clientMaxBodySize = serverConfig.clientMaxBodySize;
}

LocationConfig::LocationConfig() { }


void	LocationConfig::setRoot(const std::string &rootValue)
{
	if (!rootValue.empty())
		this->root = rootValue;
}

void	LocationConfig::setIndex(const std::string &indexValue)
{
	this->index = indexValue;
}

void	LocationConfig::setAutoindex(const std::string &autoindexValue)
{
	if (autoindexValue.empty() || (autoindexValue != "on" && autoindexValue != "off"))
		throw (std::runtime_error("invalid value \"" + autoindexValue + "\" in \"autoindex\" directive, it must be \"on\" or \"off\""));
	this->autoindex = autoindexValue;
}

void	LocationConfig::setErrorPage(const std::string &statusCode, const std::string &uri)
{
	if (statusCode.empty() || statusCode.size() > 3)
		throw std::runtime_error("invalid code in \"error_page\" directive: \"" + statusCode + "\"");
	for (size_t j = 0; j < statusCode.size(); j++)
	{
		if (!std::isdigit(statusCode[j]))
			throw std::runtime_error("invalid code in \"error_page\" directive: \"" + statusCode + "\"");
	}
	int codeInt = std::stoi(statusCode);
	if (codeInt < 300 || codeInt > 599)
		throw std::runtime_error("invalid code in \"error_page\" directive: \"" + statusCode + "\"" + " (must be between 300 and 599)");
	errorPages[codeInt] = uri;
}

void	LocationConfig::processFallbackStatusCode(const std::string &statusCode)
{
	std::string code = statusCode.substr(1);
	if (code.empty() || code.size() > 3)
		throw std::runtime_error("invalid code in \"try_files\" directive: \"" + statusCode + "\"");
	for (size_t j = 0; j < code.size(); j++)
	{
		if (!std::isdigit(code[j]))
			throw std::runtime_error("invalid code in \"try_files\" directive: \"" + statusCode + "\"");
	}
	int codeInt = std::stoi(code);
	if (codeInt < 300 || codeInt > 599)
		throw std::runtime_error("invalid code in \"try_files\" directive: \"" + statusCode + "\"" + " (must be between 300 and 599)");
	this->tryFiles.setFallBackStatusCode(codeInt);
}

size_t	LocationConfig::safeStringToSizeT(const std::string &bodySize, const std::string value)
{
	try
	{
		return (std::stoull(value));
		
	}
	catch (const std::exception &e)
	{
		throw (std::runtime_error("invalid value \"" + bodySize + "\" in \"client_max_body_size\" directive"));
	}
}

void	LocationConfig::splitValueAndUnit(const std::string &bodySize, std::string &value, std::string &unit)
{
	size_t pos = bodySize.find_first_not_of("0123456789");
	if (pos != std::string::npos)
	{
		value = bodySize.substr(0, pos);
		unit = bodySize.substr(pos);
	}
	else
		value = bodySize;
}

void	LocationConfig::setClientMaxBodySize(const std::string &bodySize)
{
	size_t multiplier = 1; // in case there is no unit (bytes by default)
	size_t numericValue, totalSize;
	std::string value, unit;

	splitValueAndUnit(bodySize, value, unit);

	if (value.empty() || unit.size() > 1)
		throw (std::runtime_error("invalid value \"" + bodySize + "\" in \"client_max_body_size\" directive"));
	if (unit.size() == 1 && unit.find_first_not_of("kKmMgG") != std::string::npos)
		throw (std::runtime_error("invalid value \"" + bodySize + "\" in \"client_max_body_size\" directive"));
	if (unit == "k" || unit == "K")
		multiplier = 1000;
	else if (unit == "m" || unit == "M")
		multiplier = 1000000;
	else if (unit == "g" || unit == "G")
		multiplier = 1000000000;
	
	numericValue = safeStringToSizeT(bodySize, value);
	totalSize = numericValue * multiplier;
	// check for overflow
	if (totalSize / multiplier != numericValue)
		throw (std::runtime_error("invalid value \"" + bodySize + "\" in \"client_max_body_size\" directive"));
}

void	LocationConfig::setTryFiles(const std::vector<std::string> &tryFilesValue)
{
	size_t i;
	for (i = 0; i < tryFilesValue.size() - 1; i++)
		this->tryFiles.addPath(tryFilesValue[i]);
	if (tryFilesValue[i][0] == '=')
		processFallbackStatusCode(tryFilesValue[i]);
	else
		this->tryFiles.setFallBackUri(tryFilesValue[i]);
}

void	LocationConfig::setReturn(const std::vector<std::string> &returnValue)
{
	if (returnValue.size() == 2)
		this->returnDirective.setResponseTextOrUrl(returnValue[1]);
	std::string code = returnValue[0];
	if (code.empty() || code.size() > 3)
		throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"");
	for (size_t j = 0; j < code.size(); j++)
	{
		if (!std::isdigit(code[j]))
			throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"");
	}
	int codeInt = std::stoi(code);
	if (codeInt < 300 || codeInt > 599)
		throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"" + " (must be between 300 and 599)");
	this->returnDirective.setStatusCode(codeInt);
}

void	LocationConfig::setRewrite(const std::vector<std::string> &rewriteValue)
{
	this->rewriteDirective.setPattern(rewriteValue[0]);
	this->rewriteDirective.setSubstitution(rewriteValue[1]);
}