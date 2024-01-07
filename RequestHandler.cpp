#include "RequestHandler.hpp"
#include "BaseConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <cstddef>
#include <iostream>
#include <sys/_types/_size_t.h>


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


HttpResponse	RequestHandler::handleReturnDirective(HttpRequest &request, BaseConfig *config)
{
	HttpResponse	response;

	int statusCode = config->returnDirective.getStatusCode();
	const std::string &responseTextOrUrl = config->returnDirective.getResponseTextOrUrl();

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

HttpResponse	RequestHandler::handleTryFilesDirective(HttpRequest &request, BaseConfig *config)
{
	std::vector<std::string> &tryFilesPaths = config->tryFiles.getPaths();
	for (size_t i = 0; i < tryFilesPaths.size(); i++)
	{
		size_t pos = tryFilesPaths[i].find("$uri");
		if (pos != std::string::npos)
			tryFilesPaths[i].replace(pos, 4, request.getUri());
		std::string	path = config->root + tryFilesPaths[i];
		if (fileExists(path))
		{
			if (isDirectory(path) && path.back() == '/')
				return sendRedirect(request, tryFilesPaths[i]);
			else
				return serveFile(path);
		}
	}
	if (config->tryFiles.getFallBackUri().empty())
		return (serveError(config->tryFiles.getFallBackStatusCode()));
	else
	{
		if (request.getRecursionDepth() >= MAX_RECURSION_DEPTH)
			return (serveError(500));
		request.increaseRecursionDepth();
		request.setUri(config->tryFiles.getFallBackUri());
		return (handleRequest(request));
	}
}

HttpResponse	RequestHandler::handleRequest(HttpRequest &request)
{
	if (request.getStatus() != 200)
		return (serveError(request.getStatus()));
	
	if (serverConfig.returnDirective.isEnabled())
		return handleReturnDirective(request, &serverConfig);

	BaseConfig		*config = &serverConfig;
	LocationConfig	*locationConfig = serverConfig.matchLocation(request.getUri());

	if (locationConfig)
		config = locationConfig;

	if (config->returnDirective.isEnabled())
			return handleReturnDirective(request, config);

	if (config->tryFiles.isEnabled())
		return handleTryFilesDirective(request, config);
	

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
	if (statusCode >= 500 && statusCode < 600 && statusCode != 503)
		response.setHeader("Connection", "close");
	return (response);
}

