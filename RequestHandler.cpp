#include "RequestHandler.hpp"
#include "HttpResponse.hpp"
#include <iostream>
#include <string>
#include <sys/_types/_size_t.h>


RequestHandler::RequestHandler(ServerConfig &serverConfig, MimeTypeParser &mimeTypes)
	: serverConfig(serverConfig), mimeTypes(mimeTypes)
{
	
}

RequestHandler::~RequestHandler() { }

void	RequestHandler::initStatusCodeMessages()
{
	statusCodeMessages[200] = "OK";
	statusCodeMessages[201] = "Created";
	statusCodeMessages[202] = "Accepted";
	statusCodeMessages[204] = "No Content";
	statusCodeMessages[301] = "Moved Permanently";
	statusCodeMessages[302] = "Found";
	statusCodeMessages[304] = "Not Modified";
	statusCodeMessages[400] = "Bad Request";
	statusCodeMessages[401] = "Unauthorized";
	statusCodeMessages[403] = "Forbidden";
	statusCodeMessages[404] = "Not Found";
	statusCodeMessages[405] = "Method Not Allowed";
	statusCodeMessages[413] = "Payload Too Large";
	statusCodeMessages[500] = "Internal Server Error";
	statusCodeMessages[501] = "Not Implemented";
	statusCodeMessages[503] = "Service Unavailable";
	statusCodeMessages[505] = "HTTP Version Not Supported";
}


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

size_t	RequestHandler::getFileSize(const std::string &path)
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

std::string	RequestHandler::generateDirectoryListing(const std::string &uri, const std::string &path)
{
	std::string htmlContent = "<html><head><title>Index of " + uri + "</title></head><body><h1>Index of " + uri + "</h1><hr><pre>";

	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		return (htmlContent + "</pre><hr></body></html>");
	htmlContent += "<a href=\"../\">../</a><br>";
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string fileName = entry->d_name;
		if (fileName == "." || fileName == "..")
			continue;
		if (isDirectory(path + "/" + fileName))
			fileName += "/";
		htmlContent += "<a href=\"" + fileName + "\">" + fileName + "</a><br>";
	}
	htmlContent += "</pre><hr></body></html>";
	closedir(dir);
	return (htmlContent);
}

HttpResponse	RequestHandler::serveFile(const std::string &path)
{
	HttpResponse	response;

	if (!fileExistsAndAccessible(path))
	{
		return serveError(403);
	}
	else
	{
		// 200 OK
		response.setVersion("HTTP/1.1");
		response.setStatusCode("200");
		response.setStatusMessage("OK");
		size_t fileSize = getFileSize(path);
		if (fileSize <= MAX_FILE_SIZE)
		{
			std::cout << "File size is small" << std::endl;
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
			// send large file in chunks
			response.filePath = path;
			response.fileSize = fileSize;
			response.setType(LARGE_FILE);


			response.setHeader("Content-Type", mimeTypes.getMimeType(path));
			response.setHeader("Server", "Nginx 2.0");
			response.setHeader("Connection", "keep-alive");
			response.setHeader("Transfer-Encoding", "chunked");

		}
	}
	return (response);
}



HttpResponse	RequestHandler::handleRequest(const Method &request)
{
	HttpResponse	response;


	// if (serverConfig.tryFiles.isEnabled())
	// {
	// 	const std::vector<std::string> &paths = serverConfig.tryFiles.getPaths();
	// 	for (size_t i = 0; i < paths.size(); i++)
	// 	{
	// 		std::string path = serverConfig.root + paths[i];
	// 		if (fileExists(path))
	// 		{
	// 			std::cout << "File exists" << std::endl;
	// 			if (isDirectory(path))
	// 			{
	// 				std::cout << "File is a directory" << std::endl;
	// 				for (size_t i = 0; i < serverConfig.index.size(); i++)
	// 				{
	// 					std::string indexPath = path + "/" + serverConfig.index[i];
	// 					if (fileExists(indexPath))
	// 					{
	// 						std::cout << "Index file exists" << std::endl;
	// 						return serveFile(indexPath);
	// 					}
	// 				}
	// 				// handle autoindex directive
	// 				if (serverConfig.autoindex == "off")
	// 				{
	// 					std::cout << "Autoindex is off -> 403" << std::endl;
	// 					// // 403 Forbidden
	// 					response.setVersion("HTTP/1.1");
	// 					response.setStatusCode("403");
	// 					response.setStatusMessage("Forbidden");
	// 					response.setBody("<html><body><h1>403 Forbidden</h1></body></html>");
	// 					response.setHeader("Content-Type", "text/html");
	// 					response.setHeader("Content-Length", std::to_string(response.getBody().length()));
	// 					response.setHeader("Server", "Nginx 2.0");
	// 					response.setHeader("Connection", "close");
	// 				}
	// 				else
	// 				{
	// 					std::cout << "Autoindex is on -> 200" << std::endl;
	// 					response.setVersion("HTTP/1.1");
	// 					response.setStatusCode("200");
	// 					response.setStatusMessage("OK");
	// 					response.setBody(generateDirectoryListing(request.getUri(), path));
	// 					response.setHeader("Content-Length", std::to_string(response.getBody().length()));
	// 					response.setHeader("Content-Type", "text/html");
	// 					response.setHeader("Server", "Nginx 2.0");
	// 					response.setHeader("Connection", "keep-alive");
	// 				}
	// 			}
	// 			else
	// 			{
	// 				std::cout << "File is not a directory" << std::endl;
	// 				return serveFile(path);
	// 			}
	// 		}
	// 	}
	// }

	std::string	path = resolvePath(request.getUri());

	std::cout << "path: " << path << std::endl;


	if (fileExists(path))
	{
		std::cout << "File exists" << std::endl;
		if (isDirectory(path))
		{
			std::cout << "File is a directory" << std::endl;
			for (size_t i = 0; i < serverConfig.index.size(); i++)
			{
				std::string indexPath = path + "/" + serverConfig.index[i];
				if (fileExists(indexPath))
				{
					std::cout << "Index file exists" << std::endl;
					return serveFile(indexPath);
				}
			}
			// handle autoindex directive
			if (serverConfig.autoindex == "off")
			{
				return serveError(403);
			}
			else
			{
				std::cout << "Autoindex is on -> 200" << std::endl;
				response.setVersion("HTTP/1.1");
				response.setStatusCode("200");
				response.setStatusMessage("OK");
				response.setBody(generateDirectoryListing(request.getUri(), path));
				response.setHeader("Content-Length", std::to_string(response.getBody().length()));
				response.setHeader("Content-Type", "text/html");
				response.setHeader("Server", "Nginx 2.0");
				response.setHeader("Connection", "keep-alive");
			}
		}
		else
		{
			std::cout << "File is not a directory" << std::endl;
			return serveFile(path);
		}
	}
	else
	{
		response = serveError(404);
	}
	return (response);
}

HttpResponse	RequestHandler::serveError(int statusCode)
{
	HttpResponse	response;

	if (statusCodeMessages.find(statusCode) == statusCodeMessages.end())
		statusCode = 500;

	response.setVersion("HTTP/1.1");
	response.setStatusCode(std::to_string(statusCode));
	response.setStatusMessage(statusCodeMessages[statusCode]);
	response.setBody("<html><body><h1>" + std::to_string(statusCode) + " " + statusCodeMessages[statusCode] + "</h1></body></html>");
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Content-Length", std::to_string(response.getBody().length()));
	response.setHeader("Server", "Nginx 2.0");
	if (statusCode == 404)
		response.setHeader("Connection", "keep-alive");
	else
		response.setHeader("Connection", "close");
	return (response);
}