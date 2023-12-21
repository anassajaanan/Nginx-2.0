#include "ReturnDirective.hpp"


ReturnDirective::ReturnDirective() : statusCode(0) { }

ReturnDirective::~ReturnDirective() { }

void	ReturnDirective::setStatusCode(int statusCode)
{
	this->statusCode = statusCode;
}

void	ReturnDirective::setResponseTextOrUrl(const std::string &responseTextOrUrl)
{
	// url if status code is 301 or 302, otherwise response text
	this->responseTextOrUrl = responseTextOrUrl;
}