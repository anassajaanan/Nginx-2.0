


#pragma once
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <iostream>
#include <cstddef>
#include <cstring>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>


class HttpRequest
{

private:
	std::string							host;
	std::string							method;
	std::string							uri;
	std::string							version;
	std::map<std::string, std::string>	headers;
	std::string							body;

	
	void				seturi(const std::string &str);
	void				setVersion(const std::string &str);
	void				setRequestMethod(const std::string &str);
public:
	HttpRequest(const std::string &content);
	~HttpRequest();

	void				requestTokenizer(const std::string &requestString);
	void				validateRequesLine(const std::string &requestLine);
	void				setHost(const std::string &hostName);
	const std::string	&getHost() const;
	const std::string	&getRequestMethod() const;
	const std::string	&getVersion() const;
	const std::string	&getUri() const;
	const std::string	&getFromRequest(const std::string &key) const;
	const std::map<std::string, std::string> &getRequestContent() const;
	void				loadRequestContent(const std::vector<std::string> &requestVec);
	std::vector<std::string> splitByString(const std::string &str, const char *del);
	

};









#endif /* HTTPREQUEST_HPP */