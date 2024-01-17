#include "RequestHandler.hpp"
#include "BaseConfig.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <cstddef>
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
	statusCodeMessages[405] = "Method Not Allowed";
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

bool	RequestHandler::pathExists(std::string path)
{
	struct stat fileStat;

	size_t lastNonSlash = path.find_last_not_of('/');
	if (lastNonSlash != std::string::npos)
		path.erase(lastNonSlash + 1);

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

HttpResponse	RequestHandler::serveError(int statusCode)
{
	HttpResponse	response;

	initStatusCodeMessages();

	response.setVersion("HTTP/1.1");
	response.setStatusCode(std::to_string(statusCode));
	response.setHeader("Content-Type", "text/plain");
	if (statusCodeMessages.find(statusCode) != statusCodeMessages.end())
	{
		response.setStatusMessage(statusCodeMessages[statusCode]);
		if (statusCode >= 400 && statusCode < 600) // Client Response Error or Server Response Error
		{
			response.setHeader("Content-Type", "text/html");
			// response.setBody("<html><body><h1>" + std::to_string(statusCode)
			// 	+ " " + statusCodeMessages[statusCode] + "</h1></body></html>");
			std::string code = std::to_string(statusCode);
			std::string message = code + " " + statusCodeMessages[statusCode];
			std::string htmlBody = "<html><head><title>" + message + "</title></head>"
                           "<body><center><h1>" + message + "</h1></center>"
                           "<hr><center>nginx 2.0</center></body></html>";
			response.setBody(htmlBody);
		}
	}
	response.setHeader("Content-Length", std::to_string(response.getBody().length()));
	response.setHeader("Server", "Nginx 2.0");
	response.setHeader("Connection", "keep-alive");
	if (statusCode >= 500 && statusCode < 600 && statusCode != 503)
		response.setHeader("Connection", "close");
	return (response);
}

HttpResponse	RequestHandler::handleErrorPage(HttpRequest &request, BaseConfig *config, int statusCode)
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
		// config->errorPages.clear();
		// config->errorPagesContext.clear();
		return (handleFallbackUri(request, config, errorPageFileOrUrl));
	}
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
		return (serveError(500));
	request.incrementRecursionDepth();
	request.setUri(fallback);
	return (handleRequest(request));
}

HttpResponse	RequestHandler::serveFile(HttpRequest &request, BaseConfig *config, const std::string& path)
{
	HttpResponse	response;

	if (!fileExistsAndAccessible(path))
		return handleErrorPage(request, config, 403);
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
			response.setType(LARGE_RESPONSE);
			response.setHeader("Content-Type", mimeTypes.getMimeType(path));
			response.setHeader("Server", "Nginx 2.0");
			response.setHeader("Connection", "keep-alive");
			response.setHeader("Transfer-Encoding", "chunked");
		}
	}
	return (response);
}

HttpResponse	RequestHandler::handleAutoIndex(HttpRequest &request, BaseConfig *config)
{
	if (config->autoindex == "off")
		return handleErrorPage(request, config, 403);
	else
	 	return (serveDirectoryListing(request.getUri(), config->root + request.getUri()));
}

HttpResponse	RequestHandler::handleDirectory(HttpRequest &request, BaseConfig *config)
{
	if (request.getUri().back() != '/')
		return sendRedirect(request, request.getUri() + "/");

	// Single fallback URI case
	if (config->index.size() == 1 && config->index[0][0] == '/')
		return (handleFallbackUri(request, config, config->index[0]));
	
	// Check if requested URI is a directory
	if (!isDirectory(config->root + request.getUri()))
		return handleErrorPage(request, config, 404);

	for (size_t i = 0; i < config->index.size(); i++)
	{
		if (i == config->index.size() - 1 && config->index[i][0] == '/')
			return (handleFallbackUri(request, config, config->index[i]));
		std::string indexPath = config->root + request.getUri() + config->index[i];
		if (pathExists(indexPath))
		{
			if (isDirectory(indexPath))
				return (handleFallbackUri(request, config, request.getUri() + config->index[i]));
			else if (fileExists(indexPath))
				return serveFile(request, config, indexPath);
			else
				return (handleErrorPage(request, config, 404));
		}
	}
	return (handleAutoIndex(request, config));
}

HttpResponse	RequestHandler::servePath(HttpRequest &request, BaseConfig *config)
{
	std::string	path = config->root + request.getUri();

	if (path.back() == '/' || isDirectory(path))
		return handleDirectory(request, config);
	else if (fileExists(path))
		return serveFile(request, config, path);
	else
		return (handleErrorPage(request, config, 404));
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
			return handleErrorPage(request, config, statusCode);
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

HttpResponse RequestHandler::handleTryFilesDirective(HttpRequest &request, BaseConfig *config)
{
	std::string				tryFilesPath;
	std::string				expandedUri;
	const std::vector<std::string> &tryFilesParameters = config->tryFiles.getPaths();

	for (size_t i = 0; i < tryFilesParameters.size(); i++)
	{
		if (tryFilesParameters[i] == "$uri")
			continue;
		expandedUri = tryFilesParameters[i];
		replaceUri(expandedUri, "$uri", request.getUri());
		tryFilesPath = config->root + expandedUri;
		if (tryFilesParameters[i] == "$uri/")
		{
			if (i > 0 && tryFilesParameters[i - 1] == "$uri")
				return (handleDirectory(request, config));
			if (request.getUri().back() != '/')
				return sendRedirect(request, expandedUri);
		}
		else if (isDirectory(tryFilesPath))
		{
			if (tryFilesPath.back() == '/')
				return sendRedirect(request, expandedUri);
		}
		else if (fileExists(tryFilesPath))
			return (serveFile(request, config, tryFilesPath));
	}
	if(config->tryFiles.getFallBackUri().empty())
		return (handleErrorPage(request, config, config->tryFiles.getFallBackStatusCode()));
	else
	{
		std::string fallbackUri = config->tryFiles.getFallBackUri();
		replaceUri(fallbackUri, "$uri", request.getUri());
		return (handleFallbackUri(request, config, fallbackUri));
	}
}

HttpResponse	RequestHandler::handleGetRequest(HttpRequest &request)
{
	
	if (serverConfig.returnDirective.isEnabled())
		return handleReturnDirective(request, &serverConfig);

	BaseConfig		*config = &serverConfig;
	LocationConfig	*locationConfig = serverConfig.matchLocation(request.getUri());

	if (locationConfig)
	{
		config = locationConfig;
		if (locationConfig->isMethodAllowed(request.getMethod()) == false)
			return (serveError(405));
	}

	if (config->returnDirective.isEnabled())
			return handleReturnDirective(request, config);

	if (config->tryFiles.isEnabled())
		return (handleTryFilesDirective(request, config));

	return (servePath(request, config));
	
}

HttpResponse	RequestHandler::handleRequest(HttpRequest &request)
{
	if (request.getStatus() != 200)
		return (serveError(request.getStatus()));
	
	std::cerr << "RequestHandler is handling a request of type: " << request.getMethod() << std::endl;
	if (request.getMethod() == "GET")
		return (handleGetRequest(request));
	else if (request.getMethod() == "POST")
	{
		HttpResponse response;

		response.setVersion("HTTP/1.1");
		response.setStatusCode("200");
		response.setStatusMessage("OK");
		std::string body = "<h2>Your Post request was successful received and handled</h2>";
		response.setBody(body);
		response.setHeader("Content-Length", std::to_string(body.length()));
		response.setHeader("Content-Type", "text/html");
		response.setHeader("Server", "Nginx 2.0");
		response.setHeader("Connection", "keep-alive");
		return (response);
	}
	else {
		return (serveError(405));
	}


	// return (handleGetRequest(request));
}

