#include "RequestHandler.hpp"
#include "MimeTypeParser.hpp"
#include <fstream>


RequestHandler::RequestHandler(ServerConfig &serverConfig, MimeTypeParser &mimeTypes)
	: serverConfig(serverConfig), mimeTypes(mimeTypes)
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

long	RequestHandler::getFileSize(const std::string &path)
{
	struct stat fileStat;

	if (stat(path.c_str(), &fileStat) == 0)
		return (fileStat.st_size);
	return (-1);
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
		response.setHeader("Server", "Nginx 2.0");
		response.setHeader("Connection", "keep-alive");
	}
	else
	{
		// 200 OK
		response.setVersion("HTTP/1.1");
		response.setStatusCode("200");
		response.setStatusMessage("OK");
		long fileSize = getFileSize(path);
		if (fileSize <= MAX_IN_MEMORY_SIZE)
		{
			std::ifstream file(path, std::ios::binary);
			std::string content = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			response.setBody(content);
			response.setHeader("Content-Length", std::to_string(response.getBody().length()));
			response.setHeader("Content-Type", mimeTypes.getMimeType(path));
			response.setHeader("Server", "Nginx 2.0");
			response.setHeader("Connection", "keep-alive");
			file.close();
		}
		else
		{
			
		}
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