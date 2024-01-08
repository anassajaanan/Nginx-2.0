#include "RequestHandler.hpp"
#include "BaseConfig.hpp"
#include "HttpResponse.hpp"
#include <string>


RequestHandler::RequestHandler(ServerConfig &serverConfig, MimeTypeParser &mimeTypes)
	: serverConfig(serverConfig), mimeTypes(mimeTypes) { }

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

bool	RequestHandler::isRedirectStatusCode(int statusCode)
{
	if (statusCode == 301 || statusCode == 302 || statusCode == 303
		|| statusCode == 307 || statusCode == 308)
		return (true);
	return (false);
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

HttpResponse	RequestHandler::serveDirectoryListing(const std::string &uri, const std::string &path)
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

HttpResponse	RequestHandler::handleFallbackUri(HttpRequest &request, BaseConfig *config, const std::string &fallback)
{
	if (request.getRecursionDepth() >= MAX_RECURSION_DEPTH)
		return (serveErrorPage(request, config, 500));
	request.incrementRecursionDepth();
	request.setUri(fallback);
	return (handleRequest(request));
}

HttpResponse	RequestHandler::serveFile(HttpRequest &request, BaseConfig *config, const std::string& path)
{
	HttpResponse	response;

	if (!fileExistsAndAccessible(path))
		return serveErrorPage(request, config, 403);
	else
	{
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

HttpResponse	RequestHandler::handleDirectory(HttpRequest &request, BaseConfig *config)
{
	if (request.getUri().back() != '/')
		return sendRedirect(request, request.getUri() + "/");
	for (size_t i = 0; i < config->index.size(); i++)
	{
		if (i == config->index.size() - 1 && config->index[i][0] == '/')
			return (handleFallbackUri(request, config, config->index[i]));
		std::string indexPath;
		if (isDirectory(config->root + request.getUri()))
			indexPath = config->root + request.getUri()  + config->index[i];
		else
			indexPath = config->root + request.getUri() + "/" + config->index[i];
		if (fileExists(indexPath))
		{
			if (isDirectory(indexPath))
			{
				request.setUri(request.getUri() + config->index[i]);
				return handleDirectory(request, config);
			}
			else
				return serveFile(request, config, indexPath);
		}
	}
	if (config->autoindex == "off")
		return serveErrorPage(request, config, 403);
	else
		return (serveDirectoryListing(request.getUri(), config->root + request.getUri()));
}



HttpResponse	RequestHandler::handleReturnDirective(HttpRequest &request, BaseConfig *config)
{
	HttpResponse	response;

	initStatusCodeMessages();
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
			return serveErrorPage(request, config, statusCode);
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

// HttpResponse	RequestHandler::handleTryFilesDirective(HttpRequest &request, BaseConfig *config)
// {
// 	std::vector<std::string> &tryFilesPaths = config->tryFiles.getPaths();
// 	for (size_t i = 0; i < tryFilesPaths.size(); i++)
// 	{
// 		size_t pos = tryFilesPaths[i].find("$uri");
// 		if (pos != std::string::npos)
// 			tryFilesPaths[i].replace(pos, 4, request.getUri());
// 		std::string	path = config->root + tryFilesPaths[i];
// 		if (fileExists(path))
// 		{
// 			if (isDirectory(path) && path.back() == '/')
// 				return sendRedirect(request, tryFilesPaths[i]);
// 			else
// 				return serveFile(path);
// 		}
// 	}
// 	if (config->tryFiles.getFallBackUri().empty())
// 		return (serveError(config->tryFiles.getFallBackStatusCode()));
// 	else
// 		return (handleFallbackUri(request, config->tryFiles.getFallBackUri()));
// }

HttpResponse	RequestHandler::handleRequest(HttpRequest &request)
{
	if (request.getStatus() != 200)
		return (serveErrorPage(request, &serverConfig, request.getStatus()));
	
	if (serverConfig.returnDirective.isEnabled())
		return handleReturnDirective(request, &serverConfig);

	BaseConfig		*config = &serverConfig;
	LocationConfig	*locationConfig = serverConfig.matchLocation(request.getUri());

	if (locationConfig)
		config = locationConfig;

	if (config->returnDirective.isEnabled())
			return handleReturnDirective(request, config);

	// if (config->tryFiles.isEnabled())
	// {
		// std::string				tryFilesPath = config->root + request.getUri();
		// std::vector<std::string> tryFilesParameters = config->tryFiles.getPaths();
		// std::vector<std::string>::iterator it = tryFilesParameters.begin();
		// it = tryFilesParameters.begin();
		// std::string				expandedUri;
		// if (it != (config->tryFiles.getPaths()).end())
		// {			int counter = 0;
		// 	while (counter < tryFilesParameters.size())
		// 	{
		// 		expandedUri = tryFilesParameters[counter];
		// 		replaceUri(expandedUri, "$uri", request.getUri());
		// 		tryFilesPath = config->root + expandedUri;
		// 		if (tryFilesPath.back() == '/' && tryFilesPath.back() - 1 == '/')
		// 			tryFilesPath = tryFilesPath.substr(0, tryFilesPath.length() - 1);
		// 		if (tryFilesParameters[counter] == "$uri/") //remove extra slash
		// 			tryFilesPath = tryFilesPath.substr(0, tryFilesPath.length() - 1);
		// 		if (tryFilesParameters[counter] == "$uri")
		// 		{
		// 			if (!isDirectory(tryFilesPath) && fileExists(tryFilesPath))
		// 				return (serveFile(tryFilesPath));
		// 		}
		// 		else if (tryFilesParameters[counter] == "$uri/")
		// 		{
		// 			if (isDirectory(tryFilesPath))
		// 				return (serveDirectoryTryFiles(config, request.getUri(), tryFilesPath, request));
		// 		}
		// 		else
		// 		{
		// 			if (request.getRecursionDepth() >= MAX_RECURSION_DEPTH)
		// 				return (serveError(505));
		// 			request.incrementRecursionDepth();
		// 			request.setUri(config->tryFiles.getFallBackUri());
		// 			 return (handleRequest(request));
		// 		}
		// 		counter++;
		// 	}
		// 	if(config->tryFiles.getFallBackUri().empty())
		// 		return (serveError(config->tryFiles.getFallBackStatusCode()));
		// 	else
		// 	{
		// 		if (isDirectory(config->tryFiles.getFallBackUri()))
		// 		{
		// 			if (request.getRecursionDepth() >= MAX_RECURSION_DEPTH)
		// 					return (serveError(505));
		// 			request.incrementRecursionDepth();
		// 			request.setUri(config->tryFiles.getFallBackUri());
		// 			 return (handleRequest(request));
		// 		}
		// 		return (serveFile(config->root + "/" + config->tryFiles.getFallBackUri()));
		// 	}
		// }
	// }
	

	std::string	path = config->root + request.getUri();
	if (!fileExists(path))
		return serveErrorPage(request, config, 404);
	if (isDirectory(path))
		return handleDirectory(request, config);
	else
		return serveFile(request, config, path);
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

HttpResponse	RequestHandler::serveErrorPage(HttpRequest &request, BaseConfig *config, int statusCode)
{
	if (config->errorPages.find(statusCode) == config->errorPages.end())
		return serveError(statusCode);

	const std::string &errorPageFileOrUrl = config->errorPages[statusCode];
	if (errorPageFileOrUrl[0] != '/')
	{
		HttpResponse	response;

		response.setVersion("HTTP/1.1");
		response.setStatusCode(std::to_string(302));
		response.setStatusMessage("Found");
		response.setHeader("Location", errorPageFileOrUrl);
		response.setHeader("Content-Length", "0");
		response.setHeader("Content-Type", "text/html");
		response.setHeader("Server", "Nginx 2.0");
		response.setHeader("Connection", "keep-alive");
		return (response);
	}
	else
	{
		// internal redirection
		return (handleFallbackUri(request, config, errorPageFileOrUrl));
	}
}

// handle try_files directive

// void	RequestHandler::replaceUri(std::string &str, const std::string &replace, const std::string &to)
// {
// 	size_t	i = 0;

// 	i = str.find(replace, i);
// 	while (i != std::string::npos)
// 	{
// 		str.replace(i, replace.length(), to);
// 		i += replace.length();
// 		i = str.find(replace, i);
// 	}
// }

// HttpResponse	RequestHandler::serveDirectoryTryFiles(BaseConfig *config, const std::string &uri, const std::string &path, HttpRequest &request)
// {
// 	std::vector<std::string>::iterator	it = config->index.begin();
// 	std::string							indexPath;

// 	for (;it != config->index.end(); it++)
// 	{
// 		indexPath = config->root + uri + *it;
// 		std::cout << "index = " << indexPath << std::endl;
// 		if (!isDirectory(indexPath) && fileExists(indexPath))
// 			return (serveFile(indexPath));
// 	}
// 	// if (autoindex is on serve it)
// 	// 	return (autoindex);
// 	std::cout << "issue" << std::endl;
// 	return (serveError(403));
// }

// HttpResponse RequestHandler::handleTryFilesDirective(HttpRequest &request, BaseConfig *config)
// {
// 	std::string				tryFilesPath = config->root + request.getUri();
// 		std::vector<std::string> tryFilesParameters = config->tryFiles.getPaths();
// 		std::vector<std::string>::iterator it = tryFilesParameters.begin();
// 		it = tryFilesParameters.begin();
// 		std::string				expandedUri;
// 		if (it != (config->tryFiles.getPaths()).end())
// 		{			int counter = 0;
// 			while (counter < tryFilesParameters.size())
// 			{
// 				expandedUri = tryFilesParameters[counter];
// 				replaceUri(expandedUri, "$uri", request.getUri());
// 				tryFilesPath = config->root + expandedUri;
// 				if (tryFilesPath.back() == '/' && tryFilesPath.back() - 1 == '/')
// 					tryFilesPath = tryFilesPath.substr(0, tryFilesPath.length() - 1);
// 				if (tryFilesParameters[counter] == "$uri/") //remove extra slash
// 					tryFilesPath = tryFilesPath.substr(0, tryFilesPath.length() - 1);
// 				if (tryFilesParameters[counter] == "$uri")
// 				{
// 					if (!isDirectory(tryFilesPath) && fileExists(tryFilesPath))
// 						return (serveFile(tryFilesPath));
// 				}
// 				else if (tryFilesParameters[counter] == "$uri/")
// 				{
// 					if (isDirectory(tryFilesPath))
// 						return (serveDirectoryTryFiles(config, request.getUri(), tryFilesPath, request));
// 				}
// 				else
// 				{
// 					if (request.getRecursionDepth() >= MAX_RECURSION_DEPTH)
// 						return (serveError(505));
// 					request.incrementRecursionDepth();
// 					request.setUri(config->tryFiles.getFallBackUri());
// 					 return (handleRequest(request));
// 				}
// 				counter++;
// 			}
// 			if(config->tryFiles.getFallBackUri().empty())
// 				return (serveError(config->tryFiles.getFallBackStatusCode()));
// 			else
// 			{
// 				if (isDirectory(config->tryFiles.getFallBackUri()))
// 				{
// 					if (request.getRecursionDepth() >= MAX_RECURSION_DEPTH)
// 							return (serveError(505));
// 					request.incrementRecursionDepth();
// 					request.setUri(config->tryFiles.getFallBackUri());
// 					 return (handleRequest(request));
// 				}
// 				return (serveFile(config->root + "/" + config->tryFiles.getFallBackUri()));
// 			}
// 		}
// 		return (serveFile(tryFilesPath));
// }