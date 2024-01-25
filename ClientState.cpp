#include "ClientState.hpp"
#include "Logger.hpp"
#include <sstream>
#include <string>

ClientState::ClientState(int fd, const std::string &clientIpAddr)
	: fd(fd), clientIpAddr(clientIpAddr), requestCount(0), areHeaderComplete(false), isBodyComplete(false)
{
	this->lastRequestTime = std::chrono::steady_clock::now();
}

ClientState::~ClientState()
{
	if (requestBodyFile.is_open())
		requestBodyFile.close();
}

void	ClientState::resetClientState()
{
	requestHeaders.clear();
	requestBody.clear();
	if (requestBodyFile.is_open())
		requestBodyFile.close();
	requestBodySize = 0;
	requestBodyFilePath.clear();
	areHeaderComplete = false;
	isBodyComplete = false;
}

void	ClientState::updateLastRequestTime()
{
	lastRequestTime = std::chrono::steady_clock::now();
}

void	ClientState::incrementRequestCount()
{
	requestCount++;
}

void	ClientState::processIncomingData(Server &server, const char *buffer, size_t bytesRead)
{
	if (!areHeaderComplete)
		processHeaders(server, buffer, bytesRead);
	else
		processBody(server, buffer, bytesRead);
}

void	ClientState::processHeaders(Server &server, const char *buffer, size_t bytesRead)
{
	requestHeaders.append(buffer, bytesRead);
	if (requestHeaders.size() > MAX_REQUEST_HEADERS_SIZE)
	{
		server.handleHeaderSizeExceeded(fd);
		return;
	}
	if (headersCompleted(buffer))
	{
		areHeaderComplete = true;
		Logger::log(Logger::DEBUG, "Headers completed for fd " + std::to_string(fd), "ClientState::processHeaders");
		parseHeaders(server);
	}
	else
		Logger::log(Logger::DEBUG, "Received partial headers for fd " + std::to_string(fd), "ClientState::processHeaders");
}

void	ClientState::parseHeaders(Server &server)
{
	size_t endOfHeaders = requestHeaders.find("\r\n\r\n") + 4;
	requestBody = requestHeaders.substr(endOfHeaders);
	requestHeaders.resize(endOfHeaders);

	this->request = HttpRequest(requestHeaders);

	if (request.getUri().size() > MAX_URI_SIZE)
	{
		server.handleUriTooLarge(fd);
		return;
	}

	if (request.getMethod() == "GET")
	{
		
		std::ostringstream logStream;
		logStream << "Received a GET request for '" << request.getUri() << "' from IP '"
				<< clientIpAddr << "', processing on socket descriptor " << fd;
		Logger::log(Logger::INFO, logStream.str(), "ClientState::parseHeaders");
		handleGetRequest(server);
	}

	else if (request.getMethod() == "POST")
	{
		std::ostringstream logStream;
		logStream << "Received a POST request for '" << request.getUri() << "' from IP '"
				<< clientIpAddr << "', processing on socket descriptor " << fd;
		Logger::log(Logger::INFO, logStream.str(), "ClientState::parseHeaders");
		handlePostRequest(server);
	}
	else
	{
		Logger::log(Logger::WARN, "Unsupported HTTP method for fd " + std::to_string(fd), "ClientState::parseHeaders");
		server.handleInvalidRequest(fd, 501, "Method not implemented");
	}
}

void	ClientState::handleGetRequest(Server &server)
{

	// Log the situation where a GET request contains a body, which is unusual
	if (!requestBody.empty() || !request.getHeader("Content-Length").empty() || request.getHeader("Transfer-Encoding") == "chunked")
		server.handleInvalidGetRequest(fd);
	else
		server.processGetRequest(fd, request);
}



void	ClientState::handlePostRequest(Server &server)
{
	if (request.getStatus() != 200)
	{
		Logger::log(Logger::WARN, "Invalid POST request status for client with socket fd " + std::to_string(fd), "ClientState::handlePostRequest");
		server.handleInvalidRequest(fd, request.getStatus());
		return;
	}
	if (request.getHeader("Transfer-Encoding") == "chunked")
	{
		Logger::log(Logger::WARN, "Chunked Transfer-Encoding not supported for client with socket fd " + std::to_string(fd), "ClientState::handlePostRequest");
		server.handleInvalidRequest(fd, 501, "Chunked Transfer-Encoding Not Supported for Client Requests");
		return;
	}

	requestBodySize = std::stoull(request.getHeader("Content-Length"));
	if (requestBodySize > server._config.clientMaxBodySize)
	{
		Logger::log(Logger::WARN, "Body size of POST request exceeds client max body size for client with socket fd " + std::to_string(fd), "ClientState::handlePostRequest");
		server.handleInvalidRequest(fd, 413);
		return;
	}
	initializeBodyStorage(server);
}

void	ClientState::initializeBodyStorage(Server &server)
{
	std::string	b = "body";
	std::string filename = "post_body_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "_" + std::to_string(fd) + ".tmp";
	requestBodyFilePath = TEMP_FILE_DIRECTORY + filename;

	requestBodyFile.open(requestBodyFilePath.c_str(), std::ios::out | std::ios::binary);
	if (!requestBodyFile.is_open())
	{
		Logger::log(Logger::ERROR, "Failed to open temporary file for storing POST body for client with socket fd " + std::to_string(fd), "ClientState::initializeBodyStorage");
		server.handleInvalidRequest(fd, 500, "Internal Server Error: Temporary File Creation Failed");
		return;
	}

	Logger::log(Logger::DEBUG, "Temporary file for POST body created: " + requestBodyFilePath, "ClientState::initializeBodyStorage");
	
	if (requestBody.size() == requestBodySize)
	{
		Logger::log(Logger::DEBUG, "POST request body is complete from the first read for client with socket fd " + std::to_string(fd), "ClientState::initializeBodyStorage");
		requestBodyFile << requestBody;
		requestBodyFile.close();
		isBodyComplete = true;
		server.processPostRequest(fd, request);
	}
	else if (requestBody.size() > requestBodySize)
	{
		Logger::log(Logger::WARN, "POST request body exceeds the declared content length for client with socket fd " + std::to_string(fd), "ClientState::initializeBodyStorage");
		requestBodyFile.close();
		server.handleInvalidRequest(fd, 400, "Request Body Exceeds Content-Length");
	}
}

void	ClientState::processBody(Server &server, const char *buffer, size_t bytesRead)
{
	Logger::log(Logger::DEBUG, "Processing body of POST request for client with socket fd " + std::to_string(fd), "ClientState::processBody");

	size_t remainingBodySize = requestBodySize - requestBodyFile.tellp();
	if (bytesRead > remainingBodySize)
	{
		Logger::log(Logger::WARN, "POST request body exceeds declared content length for client with socket fd " + std::to_string(fd), "ClientState::processBody");
		requestBodyFile.write(buffer, remainingBodySize);
		requestBodyFile.close();
		isBodyComplete = true;
		server.processPostRequest(fd, request, true);
		server.removeClient(fd);
	}
	else if (bytesRead == remainingBodySize)
	{
		 Logger::log(Logger::DEBUG, "POST request body is complete for client with socket fd " + std::to_string(fd), "ClientState::processBody");
		requestBodyFile.write(buffer, bytesRead);
		requestBodyFile.close();
		isBodyComplete = true;
		server.processPostRequest(fd, request);
	}
	else
	{
		Logger::log(Logger::DEBUG, "Appending to POST request body for client with socket fd " + std::to_string(fd), "ClientState::processBody");
		requestBodyFile.write(buffer, bytesRead);
	}
}

int		ClientState::getFd() const
{
	return fd;
}

const std::string &ClientState::getClientIpAddr() const
{
	return clientIpAddr;
}

int		ClientState::getRequestCount() const
{
	return requestCount;
}

bool	ClientState::isTimedOut(size_t keepalive_timeout) const
{
	std::chrono::seconds timeoutDuration(keepalive_timeout);
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
	return (std::chrono::duration_cast<std::chrono::seconds>(now - lastRequestTime) > timeoutDuration);
}

bool	ClientState::headersCompleted(const char *buffer) const
{
	return (strstr(buffer, "\r\n\r\n") != NULL);
}
