


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


class Method
{

private:
		std::string							requestMethod;
		std::string							uri;
		std::string							host;
		std::string							version;
		// std::string							body;
		std::map<std::string, std::string>	requestContent;
		void				setUri(const std::string &str);
		void				setVersion(const std::string &str);
		void				setRequestMethod(const std::string &str);
		void				validateUri(const std::string &str);
		void				validateHost(std::string &hostName);
		void				validateValue(std::string &hostName);
		void				checkArgsNumber(const std::string &arg);
		void				searchForHost();
		bool				checkVersionNumber(const std::string &str);
		void				validateVersion(const std::string &version);
		void				requestTokenizer(const std::string &requestString);
		void				validateRequesLine(const std::string &requestLine);
		void				loadRequestContent(const std::vector<std::string> &requestVec);
		std::vector<std::string> splitByString(const std::string &str, const char *del);
	public:
		Method(const std::string &content);
		void				setHost(const std::string &hostName);
		const std::string	&getHost() const;
		const std::string	&getRequestMethod() const;
		const std::string	&getVersion() const;
		const std::string	&getUri() const;
		const std::string	&getFromRequest(const std::string &key) const;
		const std::map<std::string, std::string> &getRequestContent() const;
		~Method();
	

};









#endif /* HTTPREQUEST_HPP */