

#pragma once
#ifndef MIMETYPEPARSER_HPP
#define MIMETYPEPARSER_HPP

#include "../config/MimeTypeConfig.hpp"

#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>

class MimeTypeParser
{
private:
	std::string							mimeTypeFileName;
	std::string							mimeTypeFileContent;
	std::vector<std::string>			mimeTypeTokens;
	

	MimeTypeParser();

public:
	MimeTypeParser(const std::string &fileName);
	~MimeTypeParser();

	void		readMimeTypeFile();
	void		tokenizeMimeTypeFile();
	void		parseMimeType(MimeTypeConfig &mimeTypeConfig);
	void		parseMimeTypeFile(MimeTypeConfig &mimeTypeConfig);
};

#endif // MIMETYPEPARSER_HPP
