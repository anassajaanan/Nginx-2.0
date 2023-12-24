#include "TryFilesDirective.hpp"


TryFilesDirective::TryFilesDirective() : fallBackStatusCode(0) { }

TryFilesDirective::~TryFilesDirective() { }

void	TryFilesDirective::addPath(const std::string &path)
{
	this->paths.push_back(path);
}

void	TryFilesDirective::setFallBackUri(const std::string &uri)
{
	this->fallBackUri = uri;
	this->fallBackStatusCode = 0;
}

void	TryFilesDirective::setFallBackStatusCode(int code)
{
	this->fallBackStatusCode = code;
	fallBackUri.clear();
}