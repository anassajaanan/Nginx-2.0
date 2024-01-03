

#pragma once
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "ServerConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "MimeTypeParser.hpp"
#include <sys/stat.h>
#include <dirent.h>


#define MAX_FILE_SIZE 16384 // 16 KB


class RequestHandler
{

private:
	ServerConfig						&serverConfig;
	MimeTypeParser						&mimeTypes;
	std::map<int, std::string>			statusCodeMessages;

public:
	RequestHandler(ServerConfig &serverConfig, MimeTypeParser &mimeTypes);
	~RequestHandler();

	void			initStatusCodeMessages();

	// facilitators
	std::string		resolvePath(const std::string& uri);
	bool			fileExists(const std::string& path);
	bool			isDirectory(const std::string& path);
	size_t			getFileSize(const std::string& path);

	std::string		generateDirectoryListing(const std::string &uri, const std::string &path);

	bool			fileExistsAndAccessible(const std::string &path);



	HttpResponse	serveFile(const std::string& path);
	HttpResponse	serveError(int statusCode);

	HttpResponse	handleRequest(const Method &request);
};















#endif /* REQUESTHANDLER_HPP */