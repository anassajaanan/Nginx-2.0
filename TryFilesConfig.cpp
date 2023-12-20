#include "TryFilesConfig.hpp"


TryFilesConfig::TryFilesConfig() : fallBackStatusCode(0) { }

TryFilesConfig::~TryFilesConfig() { }

void	TryFilesConfig::addPath(const std::string &path)
{
	this->paths.push_back(path);
}

void	TryFilesConfig::setFallBackUri(const std::string &uri)
{
	this->fallBackUri = uri;
	this->fallBackStatusCode = 0;
}

void	TryFilesConfig::setFallBackStatusCode(int code)
{
	this->fallBackStatusCode = code;
	fallBackUri.clear();
}