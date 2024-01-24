


#pragma once
#ifndef MIMETYPECONFIG_HPP
#define MIMETYPECONFIG_HPP

#include <map>
#include <iostream>

class MimeTypeConfig
{
private:
	std::map<std::string, std::string>	mimeTypeMap;

public:
	MimeTypeConfig();
	~MimeTypeConfig();

	void	addMimeType(const std::string &extension, const std::string &mimeType);

	std::string	getMimeType(const std::string &filePath);

};


#endif /* MIMETYPECONFIG_HPP */