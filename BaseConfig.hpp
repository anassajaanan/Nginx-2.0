

#pragma once
#ifndef BASECONFIG_HPP
# define BASECONFIG_HPP


#include "ContextNode.hpp"
#include "DirectiveNode.hpp"
#include "TryFilesDirective.hpp"
#include "ReturnDirective.hpp"
#include "RewriteDirective.hpp"

#include <sstream>


class BaseConfig
{

private:

	bool					isValidAutoindex(const std::string &autoindexValue);
	void					processFallbackStatusCode(const std::string &statusCode);
	void					splitValueAndUnit(const std::string &bodySize, std::string &value, std::string &unit);
	size_t					safeStringToSizeT(const std::string &bodySize, const std::string value);

public:
	std::string								root;
	std::string								index;
	std::string								autoindex;
	std::map<int, std::string>				errorPages;
	size_t									clientMaxBodySize;
	TryFilesDirective						tryFiles;
	ReturnDirective							returnDirective;
	RewriteDirective						rewriteDirective;

	void					setRoot(const std::string &rootValue);
	void					setIndex(const std::string &indexValue);
	void					setAutoindex(const std::string &autoindexValue);
	void					setErrorPage(const std::string &statusCode, const std::string &uri);
	void					setClientMaxBodySize(const std::string &bodySize);
	void					setTryFiles(const std::vector<std::string> &tryFilesValues);
	void					setReturn(const std::vector<std::string> &returnValues);
	void					setRewrite(const std::vector<std::string> &rewriteValues);

};


#endif /* BASECONFIG_HPP */