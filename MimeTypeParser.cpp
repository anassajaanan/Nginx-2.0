#include "MimeTypeParser.hpp"
#include <stdexcept>
#include <sys/_types/_size_t.h>

MimeTypeParser::MimeTypeParser(const std::string &fileName)
	: mimeTypeFileName(fileName) { }

MimeTypeParser::~MimeTypeParser()
{
	mimeTypeTokens.clear();
	mimeTypeMap.clear();
}

void	MimeTypeParser::readMimeTypeFile()
{
	struct stat		fileStat;
	if (stat(mimeTypeFileName.c_str(), &fileStat) == 0)
	{
		if (!S_ISREG(fileStat.st_mode))
			throw std::runtime_error("Error: The specified MIME type file ('"
			+ mimeTypeFileName + "') is not a regular file. Please provide a valid file.");
	}
	else
		throw std::runtime_error("Error: Unable to find the MIME type file ('"
		+ mimeTypeFileName + "'). Please check the file path and try again.");

	std::ifstream	file(mimeTypeFileName.c_str());
	std::string		line;

	if (!file.is_open())
		throw std::runtime_error("Error: Unable to open the MIME type file ('"
		+ mimeTypeFileName + "') for reading. Please check file permissions and try again.");
	while (std::getline(file, line))
		mimeTypeFileContent += line + "\n";
	file.close();
}

void	MimeTypeParser::tokenizeMimeTypeFile()
{
	std::string					currentToken;
	std::set<char>				delimiters;
	delimiters.insert('{');
	delimiters.insert('}');
	delimiters.insert(';');

	for (std::string::iterator it = mimeTypeFileContent.begin(); it != mimeTypeFileContent.end(); it++)
	{
		char	ch = *it;
		if (delimiters.find(ch) != delimiters.end())
		{
			if (!currentToken.empty())
			{
				mimeTypeTokens.push_back(currentToken);
				currentToken.clear();
			}
			mimeTypeTokens.push_back(std::string(1, ch));
		}
		else if (std::isspace(ch))
		{
			if (!currentToken.empty())
			{
				mimeTypeTokens.push_back(currentToken);
				currentToken.clear();
			}
		}
		else
			currentToken += ch;
	}
	if (!currentToken.empty())
		mimeTypeTokens.push_back(currentToken);


}

void	MimeTypeParser::parseMimeTypeFile()
{
	readMimeTypeFile();
	tokenizeMimeTypeFile();

	if (mimeTypeTokens.size() == 0)
		throw std::runtime_error("Error: The MIME type file ('"
		+ mimeTypeFileName + "') is empty. Please provide a valid file.");
	if (mimeTypeTokens[0] != "types")
		throw (std::runtime_error("unkown directive \"" + mimeTypeTokens[0] + "\" in MIME type file"));
	if (mimeTypeTokens[1] != "{")
		throw (std::runtime_error("directive \"types\" has no \"{\" in MIME type file"));

	size_t i = 2;
	while (i < mimeTypeTokens.size() && mimeTypeTokens[i] != "}")
	{
		if (mimeTypeTokens[i] == "{")
			throw (std::runtime_error("unexpected \"{\" in MIME type file"));
		if (mimeTypeTokens[i] == ";")
			throw (std::runtime_error("unexpected \";\" in MIME type file"));
		std::string type = mimeTypeTokens[i++];
		while (i < mimeTypeTokens.size() && mimeTypeTokens[i] != ";" && mimeTypeTokens[i] != "{" && mimeTypeTokens[i] != "}")
		{
			mimeTypeMap[mimeTypeTokens[i]] = type;
			i++;
		}
		if (i == mimeTypeTokens.size())
			throw (std::runtime_error("unexpected end of file in MIME type file"));
		if (mimeTypeTokens[i] == ";")
			i++;
		else
			throw (std::runtime_error("unexpected \"" + mimeTypeTokens[i] + "\" in MIME type file"));
	}
	if (i == mimeTypeTokens.size())
		throw (std::runtime_error("unexpected end of file in MIME type file"));
	if (i != mimeTypeTokens.size() - 1)
		throw (std::runtime_error("unexpected \"" + mimeTypeTokens[i + 1] + "\" in MIME type file"));
}

void	MimeTypeParser::printMimeTypeTokens()
{
	for (std::vector<std::string>::iterator it = mimeTypeTokens.begin(); it != mimeTypeTokens.end(); it++)
		std::cout << *it << std::endl;
	std::cout << "****************" << std::endl << std::endl;
}

void	MimeTypeParser::printMimeTypeMap()
{
	for (std::map<std::string, std::string>::iterator it = mimeTypeMap.begin(); it != mimeTypeMap.end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;
}