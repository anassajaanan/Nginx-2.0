#include "RequestHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"



RequestHandler::RequestHandler(ServerConfig &serverConfig)
	: serverConfig(serverConfig) { }

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



HttpResponse	RequestHandler::handleRequest(const HttpRequest &request)
{
	HttpResponse	response;

	std::string	path = resolvePath("/content/index.html");

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