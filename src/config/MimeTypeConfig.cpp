#include "MimeTypeConfig.hpp"

MimeTypeConfig::MimeTypeConfig() { }

MimeTypeConfig::~MimeTypeConfig() { }


void	MimeTypeConfig::addMimeType(const std::string &extension, const std::string &mimeType)
{
	mimeTypeMap[extension] = mimeType;
}

std::string	MimeTypeConfig::getMimeType(const std::string &filePath)
{
	std::string	extension;
	size_t		pos = filePath.find_last_of('.');
	if (pos != std::string::npos)
		extension = filePath.substr(pos + 1);
	else
		return ("application/octet-stream");
	// convert extension to lowercase
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
	std::map<std::string, std::string>::iterator it = mimeTypeMap.find(extension);
	if (it != mimeTypeMap.end())
		return (it->second);
	return ("text/plain");
}
