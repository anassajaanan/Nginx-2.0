

#pragma once
#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "ServerConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <sys/stat.h>


class RequestHandler
{

private:
	ServerConfig	&serverConfig;

public:
	RequestHandler(ServerConfig &serverConfig);
	~RequestHandler();


	// facilitators
	std::string		resolvePath(const std::string& uri);
	bool			fileExists(const std::string& path);
	bool			isDirectory(const std::string& path);

	bool			fileExistsAndAccessible(const std::string& path);
	HttpResponse	serveFile(const std::string& path);



	HttpResponse	handleRequest(const HttpRequest &request);
};















#endif /* REQUESTHANDLER_HPP */