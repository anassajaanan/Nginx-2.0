#include "TryFilesDirective.hpp"


TryFilesDirective::TryFilesDirective() : fallBackStatusCode(0), _isEnabled(false) { }

TryFilesDirective::~TryFilesDirective() { }

void	TryFilesDirective::addPath(const std::string &path)
{
	_isEnabled = true;
	this->paths.push_back(path);
}

void	TryFilesDirective::setFallBackUri(const std::string &uri)
{
	_isEnabled = true;
	this->fallBackUri = uri;
	this->fallBackStatusCode = 0;
}

void	TryFilesDirective::setFallBackStatusCode(int code)
{
	_isEnabled = true;
	this->fallBackStatusCode = code;
	fallBackUri.clear();
}

bool	TryFilesDirective::isEnabled() const
{
	return (this->_isEnabled);
}