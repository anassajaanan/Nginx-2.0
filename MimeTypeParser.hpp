

#pragma once
#ifndef MIMETYPEPARSER_HPP
#define MIMETYPEPARSER_HPP

#include <map>
#include <set>
#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <iostream>

class MimeTypeParser
{
private:
	std::string							mimeTypeFileName;
	std::string							mimeTypeFileContent;
	std::vector<std::string>			mimeTypeTokens;
	std::map<std::string, std::string>	mimeTypeMap;

	MimeTypeParser();

public:
	MimeTypeParser(const std::string &fileName);
	~MimeTypeParser();

	void	readMimeTypeFile();
	void	tokenizeMimeTypeFile();
	void	parseMimeTypeFile();

	void	printMimeTypeTokens();
	void	printMimeTypeMap();

	// Getters
	std::map<std::string, std::string>	&getMimeTypeMap();


};







#endif // MIMETYPEPARSER_HPP
