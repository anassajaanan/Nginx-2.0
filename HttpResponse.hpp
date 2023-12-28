

#pragma once
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>


class HttpResponse
{

private:

	std::string							version;
	std::string							statusCode;
	std::string							statusMessage;
	std::map<std::string, std::string>	headers;
	std::string							body;

public:

	HttpResponse();


	void	setVersion(const std::string& version);
	void	setStatusCode(const std::string& statusCode);
	void	setStatusMessage(const std::string& statusMessage);
	void	setHeader(const std::string& key, const std::string& value);
	void	setBody(const std::string& body);


	std::string	getVersion() const;
	std::string	getStatusCode() const;
	std::string	getStatusMessage() const;
	std::string	getHeader(const std::string& key) const;
	std::string	getBody() const;


	std::string getStatusLine() const;
	std::string getHeadersAsString() const;

	std::string buildResponse() const;



};
















#endif /* HTTPRESPONSE_HPP */