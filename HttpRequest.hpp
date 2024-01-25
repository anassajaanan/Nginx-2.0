


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
		std::vector<std::string>			queries;
		
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
		bool						validatePostRequirements();
		bool						checkVersionNumber(const std::string &str);
		bool						validateVersion(const std::string &version);
		bool						requestTokenizer(const std::string &requestString);
		bool						validateRequestLine(const std::string &requestLine);
		bool						loadRequestContent(const std::vector<std::string> &requestVec);
		std::vector<std::string>	splitByString(const std::string &str, const char *del);
		bool						checkDuplicatedHost();
		std::vector<std::string>	parseQueryString(const std::string &uri);

		void				setStatus(const int statusNum);
		void			replaceUri(std::string &str, const std::string &replace, const std::string &replaceBy);
public:
	HttpRequest();
	HttpRequest(const std::string &requestStr);
	~HttpRequest();


	const std::string	&getMethod() const;
	std::string			&getUri();
	void				setUri(const std::string &str);
	const std::string	&getVersion() const;
	const std::string	&getHost() const;
	const std::string	&getHeader(const std::string &key) const;
	const std::vector<std::string>	&getQueries() const;
	const std::map<std::string, std::string>	&getHeaders() const;

	int				getRecursionDepth() const;
	void			incrementRecursionDepth();
	void			setRecursionDepth(int depth);
	
	
	int			getStatus() const;


	void		normalizeUri();


};









#endif /* HTTPREQUEST_HPP */