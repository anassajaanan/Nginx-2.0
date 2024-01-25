#include "ReturnDirective.hpp"


ReturnDirective::ReturnDirective() : statusCode(0), _isEnabled(false) { }

ReturnDirective::~ReturnDirective() { }

void	ReturnDirective::setStatusCode(int statusCode)
{
	_isEnabled = true;
	this->statusCode = statusCode;
}

void	ReturnDirective::setResponseTextOrUrl(const std::string &responseTextOrUrl)
{
	_isEnabled = true;
	// url if status code is 301 or 302, otherwise response text
	this->responseTextOrUrl = responseTextOrUrl;
}

bool	ReturnDirective::isEnabled() const
{
	return (_isEnabled);
}

int	ReturnDirective::getStatusCode() const
{
	return (statusCode);
}

const std::string	&ReturnDirective::getResponseTextOrUrl() const
{
	return (responseTextOrUrl);
}
