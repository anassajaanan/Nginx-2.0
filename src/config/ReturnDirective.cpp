#include "ReturnDirective.hpp"


ReturnDirective::ReturnDirective() : statusCode(0), _isEnabled(false) { }

ReturnDirective::~ReturnDirective() { }

void	ReturnDirective::setStatusCode(int statusCodeValue)
{
	_isEnabled = true;
	this->statusCode = statusCodeValue;
}

void	ReturnDirective::setResponseTextOrUrl(const std::string &responseTextOrUrlValue)
{
	_isEnabled = true;
	// url if status code is 301 or 302, otherwise response text
	this->responseTextOrUrl = responseTextOrUrlValue;
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
