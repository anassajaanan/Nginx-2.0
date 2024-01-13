


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

#define MAX_RECURSION_DEPTH 10


class HttpRequest
{

private:
		std::string							method;
		std::string							uri;
		std::string							version;
		std::string							host;
		std::map<std::string, std::string>	headers;
		std::string							body;
		
		int									status;

		int 								recursionDepth;
		
		void				setMethod(const std::string &str);
		void				setVersion(const std::string &str);
		void				setHost(const std::string &hostName);


		bool						validateUri(const std::string &str);
		bool						validateHost(std::string &hostName);
		bool						validateValue(std::string &hostName);
		void						checkArgsNumber(const std::string &arg);
		bool						searchForHost();
		bool						checkVersionNumber(const std::string &str);
		bool						validateVersion(const std::string &version);
		bool						requestTokenizer(const std::string &requestString);
		bool						validateRequestLine(const std::string &requestLine);
		bool						loadRequestContent(const std::vector<std::string> &requestVec);
		std::vector<std::string>	splitByString(const std::string &str, const char *del);
		bool						checkDuplicatedHost();

		void				setStatus(const int statusNum);
public:
	HttpRequest(const std::string &requestStr);
	~HttpRequest();


	const std::string	&getMethod() const;
	const std::string	&getUri() const;
	void				setUri(const std::string &str);
	const std::string	&getVersion() const;
	const std::string	&getHost() const;
	const std::string	&getHeader(const std::string &key) const;

	int				getRecursionDepth() const;
	void			incrementRecursionDepth();
	void			setRecursionDepth(int depth);
	
	
	int			getStatus() const;


	void		normalizeUri();


};









#endif /* HTTPREQUEST_HPP */