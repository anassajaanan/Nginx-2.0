#include "BaseConfig.hpp"
#include <string>


void	BaseConfig::setRoot(const std::string &rootValue)
{
	if (rootValue.back() == '/')
		this->root = rootValue.substr(0, rootValue.size() - 1);
	else
		this->root = rootValue;
}

void		BaseConfig::setIndex(const std::vector<std::string> &indexValues)
{
	for (size_t i = 0; i < indexValues.size() - 1; i++)
	{
		if (indexValues[i][0] == '/')
			throw std::runtime_error("Only the last index in \"index\" directive should be an absolute path");
	}
	this->index = indexValues;
}

bool	BaseConfig::isValidAutoindex(const std::string &autoindexValue)
{
	if (autoindexValue.empty() || (autoindexValue != "on" && autoindexValue != "off"))
		throw (std::runtime_error("invalid value \"" + autoindexValue + "\" in \"autoindex\" directive, it must be \"on\" or \"off\""));
	return (true);
}


void	BaseConfig::setAutoindex(const std::string &autoindexValue)
{
	isValidAutoindex(autoindexValue);
	this->autoindex = autoindexValue;
}

void	BaseConfig::setErrorPage(const std::string &statusCode, const std::string &uri, const std::string &currentContext)
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
	if (errorPages.find(codeInt) == errorPages.end())
	{
		errorPages[codeInt] = uri;
		errorPagesContext[codeInt] = currentContext;
	}
	else
	{
		if (errorPagesContext[codeInt] != currentContext)
		{
			errorPages[codeInt] = uri;
			errorPagesContext[codeInt] = currentContext;
		}
	}
}

void	BaseConfig::setErrorPage(const std::vector<std::string> &errorPageValues, const std::string &currentContext)
{
	const std::string &fileOrUri = errorPageValues.back();
	for (size_t i = 0; i < errorPageValues.size() - 1; i++)
		setErrorPage(errorPageValues[i], fileOrUri, currentContext);
}

void	BaseConfig::splitValueAndUnit(const std::string &bodySize, std::string &value, std::string &unit)
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

size_t	BaseConfig::safeStringToSizeT(const std::string &bodySize, const std::string value)
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


void	BaseConfig::setClientMaxBodySize(const std::string &bodySize)
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
	this->clientMaxBodySize = totalSize;
}

void	BaseConfig::processFallbackStatusCode(const std::string &statusCode)
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

void	BaseConfig::setTryFiles(const std::vector<std::string> &tryFilesValues)
{
	size_t i;
	for (i = 0; i < tryFilesValues.size() - 1; i++)
		this->tryFiles.addPath(tryFilesValues[i]);
	if (tryFilesValues[i][0] == '=')
		processFallbackStatusCode(tryFilesValues[i]);
	else
		this->tryFiles.setFallBackUri(tryFilesValues[i]);
}

void	BaseConfig::setReturn(const std::vector<std::string> &returnValues)
{
	if (returnValues.size() == 2)
		this->returnDirective.setResponseTextOrUrl(returnValues[1]);
	std::string code = returnValues[0];
	if (code.empty() || code.size() > 3)
		throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"");
	for (size_t j = 0; j < code.size(); j++)
	{
		if (!std::isdigit(code[j]))
			throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"");
	}
	int codeInt = std::stoi(code);
	if (codeInt < 100 || codeInt > 999)
		throw std::runtime_error("invalid code in \"return\" directive: \"" + code + "\"" + " (must be between 100 and 599)");
	this->returnDirective.setStatusCode(codeInt);
}