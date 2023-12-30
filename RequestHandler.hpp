

#pragma once
#include <string>
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "ServerConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "MimeTypeParser.hpp"
#include <sys/stat.h>
#include <dirent.h>

#define MAX_IN_MEMORY_SIZE 5242880 // 5MB


class RequestHandler
{

private:
	ServerConfig						&serverConfig;
	MimeTypeParser						&mimeTypes;

public:
	RequestHandler(ServerConfig &serverConfig, MimeTypeParser &mimeTypes);
	~RequestHandler();


	// facilitators
	std::string		resolvePath(const std::string& uri);
	bool			fileExists(const std::string& path);
	bool			isDirectory(const std::string& path);
	long			getFileSize(const std::string& path);

	std::string		generateDirectoryListing(const std::string &uri, const std::string &path);

	bool			fileExistsAndAccessible(const std::string &path);
	HttpResponse	serveFile(const std::string& path);



	HttpResponse	handleRequest(const HttpRequest &request);
};















#endif /* REQUESTHANDLER_HPP */