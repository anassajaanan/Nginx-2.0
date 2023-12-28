


#pragma once
#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>


class HttpRequest
{

private:

	std::string							method;
	std::string							uri;
	std::string							version;
	std::map<std::string, std::string>	headers;
	std::string							body;

public:

	HttpRequest();

	// void parse(const std::string& request);

};









#endif /* HTTPREQUEST_HPP */