

#pragma once
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "MimeTypeParser.hpp"
#include "Logger.hpp"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
// #include "C"


#define SMALL_FILE_THRESHOLD 2097152 // 2 MB
// define the range request response size if endByte is not specified
#define RANGE_REQUEST_SIZE 2097152 // 2 MB


class RequestHandler
{

private:


	ServerConfig						&serverConfig;
	MimeTypeConfig						&mimeTypeConfig;
	std::map<int, std::string>			statusCodeMessages;

	void			initStatusCodeMessages();
	bool			fileExists(const std::string &path);
	bool			pathExists(std::string path);
	bool			isDirectory(const std::string& path);
	size_t			getFileSize(const std::string& path);
	bool			fileExistsAndAccessible(const std::string &path);
	bool			isRedirectStatusCode(int statusCode);
	void			replaceUri(std::string &str, const std::string &replace, const std::string &to);
	bool			parseRangeHeader(HttpRequest &request, size_t &startByte, size_t &endByte, size_t fileSize);
	

public:

	RequestHandler(ServerConfig &serverConfig, MimeTypeConfig &mimeTypeConfig);
	~RequestHandler();

	std::string		generateDirectoryListing(const std::string &uri, const std::string &path);
	
	HttpResponse	serveError(int statusCode);

	HttpResponse	handleErrorPage(HttpRequest &request, BaseConfig *config, int statusCode);

	HttpResponse	handleAutoIndex(HttpRequest &request, BaseConfig *config);

	HttpResponse	serveDirectoryListing(const std::string &uri, const std::string &path);

	HttpResponse	handleFallbackUri(HttpRequest &request, const std::string &fallback);

	HttpResponse	sendRedirect(HttpRequest &request, const std::string &url);

	
	HttpResponse	handleRangeRequest(HttpRequest& request, const std::string &path, size_t fileSize);
	HttpResponse	serveChunkedResponse(const std::string &path, size_t fileSize);
	HttpResponse	serverSmallFile(const std::string &path);
	HttpResponse	serveFile(HttpRequest &request, BaseConfig *config, const std::string& path);

	HttpResponse	handleDirectory(HttpRequest &request, BaseConfig *config);

	HttpResponse	servePath(HttpRequest &request, BaseConfig *config);

	HttpResponse	handleReturnDirective(HttpRequest &request, BaseConfig *config);

	HttpResponse	handleTryFilesDirective(HttpRequest &request, BaseConfig *config);

	HttpResponse	handleRequest(HttpRequest &request);
	HttpResponse	handleGetRequest(HttpRequest &request);
	HttpResponse	handlePostRequest(HttpRequest &request);


	// void			delete2dArray(char **str);
	// HttpResponse	serveCgiOutput(HttpRequest &request, const std::string &message);
	bool			validCgiRequest(HttpRequest &request, ServerConfig &config);
	// HttpResponse	handleCgiDirective(HttpRequest &request);
	bool			validateFileExtension(HttpRequest &request);
	// char			**initiateEnvVariables(HttpRequest &request);

};

	












#endif /* REQUESTHANDLER_HPP */
