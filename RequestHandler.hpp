

#pragma once
#include "BaseConfig.hpp"
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "MimeTypeParser.hpp"
#include <sys/stat.h>
#include <dirent.h>


#define MAX_FILE_SIZE 16384 // 16 KB


class RequestHandler
{

private:


	MimeTypeParser						&mimeTypes;
	ServerConfig						&serverConfig;
	std::map<int, std::string>			statusCodeMessages;

	void			initStatusCodeMessages();
	bool			fileExists(const std::string &path);
	bool			pathExists(std::string path);
	bool			isDirectory(const std::string& path);
	size_t			getFileSize(const std::string& path);
	bool			fileExistsAndAccessible(const std::string &path);
	bool			isRedirectStatusCode(int statusCode);
	void			replaceUri(std::string &str, const std::string &replace, const std::string &to);

public:

	RequestHandler(ServerConfig &serverConfig, MimeTypeParser &mimeTypes);
	~RequestHandler();

	std::string		generateDirectoryListing(const std::string &uri, const std::string &path);
	
	HttpResponse	serveError(int statusCode);

	HttpResponse	handleErrorPage(HttpRequest &request, BaseConfig *config, int statusCode);

	HttpResponse	handleAutoIndex(HttpRequest &request, BaseConfig *config);

	HttpResponse	serveDirectoryListing(const std::string &uri, const std::string &path);

	HttpResponse	handleFallbackUri(HttpRequest &request, BaseConfig *config, const std::string &fallback);

	HttpResponse	sendRedirect(HttpRequest &request, const std::string &url);

	HttpResponse	serveFile(HttpRequest &request, BaseConfig *config, const std::string& path);

	HttpResponse	handleDirectory(HttpRequest &request, BaseConfig *config);

	HttpResponse	servePath(HttpRequest &request, BaseConfig *config);

	HttpResponse	handleReturnDirective(HttpRequest &request, BaseConfig *config);

	HttpResponse	handleTryFilesDirective(HttpRequest &request, BaseConfig *config);

	HttpResponse	handleRequest(HttpRequest &request);

	HttpResponse	handleGetRequest(HttpRequest &request);

};















#endif /* REQUESTHANDLER_HPP */