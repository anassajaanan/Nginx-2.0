#include "RequestHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>



RequestHandler::RequestHandler(ServerConfig &serverConfig)
	: serverConfig(serverConfig)
{
	
}

RequestHandler::~RequestHandler() { }


std::string	RequestHandler::resolvePath(const std::string &uri)
{
	std::string path = serverConfig.root + uri;
	return (path);
}

bool	RequestHandler::fileExists(const std::string &path)
{
	struct stat fileStat;
	if (stat(path.c_str(), &fileStat) == 0)
		return (true);
	return (false);
}

bool	RequestHandler::isDirectory(const std::string &path)
{
	struct stat fileStat;

	if (stat(path.c_str(), &fileStat) == 0)
	{
		if (S_ISDIR(fileStat.st_mode))
			return (true);
	}
	return (false);
}


bool	RequestHandler::fileExistsAndAccessible(const std::string &path)
{
	struct stat fileStat;

	if (stat(path.c_str(), &fileStat) != 0)
		return (false);
	if (!S_ISREG(fileStat.st_mode))
		return (false);
	if ((fileStat.st_mode & S_IRUSR) == 0)
		return (false);
	return (true);
}

HttpResponse	RequestHandler::serveFile(const std::string &path)
{
	HttpResponse	response;

	if (!fileExistsAndAccessible(path))
	{
		// 403 Forbidden
		response.setVersion("HTTP/1.1");
		response.setStatusCode("403");
		response.setStatusMessage("Forbidden");
		response.setBody("<html><body><h1>403 Forbidden</h1></body></html>");
		response.setHeader("Content-Type", "text/html");
		response.setHeader("Content-Length", std::to_string(response.getBody().length()));
		response.setHeader("Connection", "close");
	}
	else {
		// response.setStatusCode(200);
		// serve file
	}
	

	return (response);
}



HttpResponse	RequestHandler::handleRequest(const HttpRequest &request)
{
	HttpResponse	response;

	std::string	path = resolvePath("/index.html");

	std::cout << "path: " << path << std::endl;


	if (fileExists(path))
	{
		std::cout << "File exists" << std::endl;
		if (isDirectory(path))
		{
			std::cout << "File is a directory" << std::endl;
			// response.setStatusCode(200);
		}
		else
		{
			std::cout << "File is not a directory" << std::endl;
			return serveFile(path);
			// response.setStatusCode(200);
			// serve file

		}
	}
	else
	{
		std::cout << "File does not exist" << std::endl;
		// response.setStatusCode(404);
	}



	return (response);
}