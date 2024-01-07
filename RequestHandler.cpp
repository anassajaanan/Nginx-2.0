#include "RequestHandler.hpp"
#include "BaseConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <cstring>
#include <sstream>
#include <string>
#include <variant>
#include <vector>


RequestHandler::RequestHandler(ServerConfig &serverConfig, MimeTypeParser &mimeTypes)
	: serverConfig(serverConfig), mimeTypes(mimeTypes)
{
	initStatusCodeMessages();
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
	statusCodeMessages[303] = "See Other";
	statusCodeMessages[304] = "Not Modified";
	statusCodeMessages[307] = "Temporary Redirect";
	statusCodeMessages[308] = "Permanent Redirect";
	statusCodeMessages[400] = "Bad Request";
	statusCodeMessages[401] = "Unauthorized";
	statusCodeMessages[403] = "Forbidden";
	statusCodeMessages[404] = "Not Found";
	statusCodeMessages[405] = "HttpRequest Not Allowed";
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

HttpResponse	RequestHandler::sendRedirect(HttpRequest &request, const std::string &url)
{
	HttpResponse	response;

	std::string locationHeader = "http://" + request.getHeader("Host") + url;
	response.setVersion("HTTP/1.1");
	response.setStatusCode("301");
	response.setStatusMessage("Moved Permanently");
	response.setHeader("Location", locationHeader);
	response.setHeader("Content-Length", "0");
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Server", "Nginx 2.0");
	response.setHeader("Connection", "keep-alive");
	return (response);
}

HttpResponse	RequestHandler::serveDirectory(BaseConfig *config, const std::string &uri, const std::string &path, HttpRequest &request)
{
	if (uri.back() != '/')
		return sendRedirect(request, uri + "/");
	for (size_t i = 0; i < config->index.size(); i++)
	{
		if (i == config->index.size() - 1 && config->index[i][0] == '/')
		{
			if (request.getRecursionDepth() >= MAX_RECURSION_DEPTH)
				return serveError(500);
			request.increaseRecursionDepth();
			request.setUri(config->index[i]);
			return handleRequest(request);
		}
		std::string indexPath = path + "/" + config->index[i];
		if (fileExists(indexPath))
		{
			if (isDirectory(indexPath))
				return serveDirectory(config, uri + config->index[i], indexPath, request);
			else 
				return serveFile(indexPath);
		}
	}
	if (config->autoindex == "off")
		return serveError(403);
	else
	{
		HttpResponse	response;
		response.setVersion("HTTP/1.1");
		response.setStatusCode("200");
		response.setStatusMessage("OK");
		response.setBody(generateDirectoryListing(uri, path));
		response.setHeader("Content-Length", std::to_string(response.getBody().length()));
		response.setHeader("Content-Type", "text/html");
		response.setHeader("Server", "Nginx 2.0");
		response.setHeader("Connection", "keep-alive");
		return (response);
	}
}

bool	RequestHandler::isRedirectStatusCode(int statusCode)
{
	if (statusCode == 301 || statusCode == 302 || statusCode == 303 || statusCode == 307 || statusCode == 308)
		return (true);
	return (false);
}

// Handle double quotes in the return directive
// HttpResponse	RequestHandler::serveReturnDirective(const LocationConfig *locationConfig, HttpRequest &request)
HttpResponse	RequestHandler::serveReturnDirective(int statusCode, const std::string &responseTextOrUrl, HttpRequest &request)
{
	HttpResponse	response;
	if (isRedirectStatusCode(statusCode))
	{
		std::string httpScheme = "http://";
		std::string locationHeader = responseTextOrUrl;

		if (responseTextOrUrl[0] == '/')
			locationHeader = httpScheme + request.getHeader("Host") + responseTextOrUrl;
		response.setVersion("HTTP/1.1");
		response.setStatusCode(std::to_string(statusCode));
		response.setStatusMessage(statusCodeMessages[statusCode]);
		response.setHeader("Location", locationHeader);
		response.setHeader("Content-Length", "0");
		response.setHeader("Content-Type", "text/html");
		response.setHeader("Server", "Nginx 2.0");
		response.setHeader("Connection", "keep-alive");
	}
	else
	{
		if (responseTextOrUrl.empty())
			return serveError(statusCode);
		response.setVersion("HTTP/1.1");
		response.setStatusCode(std::to_string(statusCode));
		response.setStatusMessage(statusCodeMessages[statusCode]);
		response.setBody(responseTextOrUrl);
		response.setHeader("Content-Length", std::to_string(response.getBody().length()));
		response.setHeader("Content-Type", "text/plain");
		response.setHeader("Server", "Nginx 2.0");
		response.setHeader("Connection", "keep-alive");
	}
	return (response);
}



HttpResponse	RequestHandler::handleRequest(HttpRequest &request)
{
	if (request.getStatus() != 200)
		return (serveError(request.getStatus()));
	
	if (serverConfig.returnDirective.isEnabled())
	{
		int statusCode = serverConfig.returnDirective.getStatusCode();
		const std::string &responseTextOrUrl = serverConfig.returnDirective.getResponseTextOrUrl();
		return serveReturnDirective(statusCode, responseTextOrUrl, request);
	}

	BaseConfig		*config = NULL;
	LocationConfig	*locationConfig = serverConfig.matchLocation(request.getUri());
	if (locationConfig)
	{
		config = locationConfig;
		// Location Level Config should process the request
		
		// check if return directive is enabled, if yes, return the response
		if (locationConfig->returnDirective.isEnabled())
		{
			int statusCode = locationConfig->returnDirective.getStatusCode();
			const std::string &responseTextOrUrl = locationConfig->returnDirective.getResponseTextOrUrl();
			return serveReturnDirective(statusCode, responseTextOrUrl, request);
		}
		
	}
	else
	{
		config = &serverConfig;
	}
	
	// std::cout << "herere" << std::endl;
	// handle try_files directive
	std::string				tryFilesPath = config->root + request.getUri();
	std::vector<std::string> tryFilesParameters = config->tryFiles.getPaths();
	// std::cout << "here = " << tryFilesParameters[0] << std::endl;
	std::vector<std::string>::iterator it = tryFilesParameters.begin();
	if (it == (config->tryFiles.getPaths()).end())
		std::cout <<"Failed" << std::endl;
	for (; it != tryFilesParameters.end(); it++)
		std::cout << "{" << *it << "}" << std::endl;
	it = tryFilesParameters.begin();
	std::string				expandedUri;
	if (it != (config->tryFiles.getPaths()).end())
	{
		std::cout << "len = " << tryFilesParameters.size() << std::endl;
		int counter = 0;
		while (counter  < tryFilesParameters.size())
		{
			// if (*it != tryFilesPath)
			// std::cout << "try = " << *it << std::endl;
			expandedUri = tryFilesParameters[counter];
			replaceUri(expandedUri, "$uri", request.getUri());
			// std::replace(expandedUri, expandedUri.end(), "$uri", request.getUri());
			// if (std::strncmp((tryFilesParameters[counter]).c_str(), "$uri", 4) == 0)
			// 	expandedUri = request.getUri();
			// expandedUri
			tryFilesPath = config->root + expandedUri;
			if (tryFilesParameters[counter] == "$uri/") //remove extra slash
			{
				std::cout << "here  " << tryFilesPath << std::endl;
				// std::cout << 
				tryFilesPath = tryFilesPath.substr(0, tryFilesPath.length() - 1);
				std::cout << "after  " << tryFilesPath << std::endl;
			}
			std::cout << "saved = " << expandedUri << std::endl;
			std::cout << "loop = " << tryFilesParameters[counter] << std::endl;
			if (tryFilesParameters[counter] == "$uri")
			{
				std::cout << "path = " << tryFilesPath << std::endl;
				if (!isDirectory(tryFilesPath) && fileExists(tryFilesPath))
					return (serveFile(tryFilesPath));
				std::cout << "after " << std::endl;
			}
			if (tryFilesParameters[counter] == "$uri/")
			{
				std::cout << "path dir = " << tryFilesPath << std::endl;
				if (isDirectory(tryFilesPath))
				{
					std::cout << "fdjk" << std::endl;
					std::vector<std::string>::iterator	it = config->index.begin();
					std::string							indexPath;

					for (;it != config->index.end(); it++)
					{
						indexPath = config->root + "/" + *it;
						std::cout << "index = " << indexPath << std::endl;
						if (!isDirectory(indexPath) && fileExists(indexPath))
							return (serveFile(indexPath));
					}
					// return (serveDirectoryTryFiles(config, request.getUri(), tryFilesPath, request));
					// return (serveDirectory(config, request.getUri(), tryFilesPath, request));
				}
			}
			// else
			// if((tryFilesParameters[counter]).find_first_not_of("=") != std::string::npos)
			// {
				
			// }
			// else
			// {
			
			// }
			counter++;
		}
		if(config->tryFiles.getFallBackUri().empty())
			return (serveError(config->tryFiles.getFallBackStatusCode()));
		else
		{
			if (request.getRecursionDepth() >= MAX_RECURSION_DEPTH)
				return (serveError(505));
			request.increaseRecursionDepth();
			request.setUri(config->tryFiles.getFallBackUri());
			handleRequest(request);
		}
	}

	std::string	path = config->root + request.getUri();
	if (!fileExists(path))
		return serveError(404);
	if (isDirectory(path))
		return serveDirectory(config, request.getUri(), path, request);
	else
		return serveFile(path);
}

HttpResponse	RequestHandler::serveError(int statusCode)
{
	HttpResponse	response;

	std::cout << "enetered here" << std::endl;
	initStatusCodeMessages();
	if (statusCodeMessages.find(statusCode) == statusCodeMessages.end())
		statusCode = 500;
	response.setVersion("HTTP/1.1");
	response.setStatusCode(std::to_string(statusCode));
	response.setHeader("Content-Type", "text/plain");
	if (statusCodeMessages.find(statusCode) != statusCodeMessages.end())
	{
		response.setStatusMessage(statusCodeMessages[statusCode]);
		if (statusCode >= 400 && statusCode < 600) // Client Response Error or Server Response Error
		{
			response.setHeader("Content-Type", "text/html");
			response.setBody("<html><body><h1>" + std::to_string(statusCode)
				+ " " + statusCodeMessages[statusCode] + "</h1></body></html>");
		}
	}
	response.setHeader("Content-Length", std::to_string(response.getBody().length()));
	response.setHeader("Server", "Nginx 2.0");
	response.setHeader("Connection", "keep-alive");
	return (response);
}

void	RequestHandler::replaceUri(std::string &str, const std::string &replace, const std::string &to)
{
	size_t	i = 0;

	i = str.find(replace, i);
	while (i != std::string::npos)
	{
		str.replace(i, replace.length(), to);
		i += replace.length();
		i = str.find(replace, i);
	}
}

HttpResponse	RequestHandler::serveDirectoryTryFiles(BaseConfig *config, const std::string &uri, const std::string &path, HttpRequest &request)
{
	std::vector<std::string>::iterator	it = config->index.begin();
	std::string							indexPath;

	for (;it != config->index.end(); it++)
	{
		indexPath = config->root + "/" + *it;
		std::cout << "index = " << indexPath << std::endl;
		if (!isDirectory(indexPath) && fileExists(indexPath))
			return (serveFile(indexPath));
	}
	return (serveError(403));
}