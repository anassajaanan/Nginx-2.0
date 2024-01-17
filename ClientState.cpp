#include "ClientState.hpp"

ClientState::ClientState(int fd)
	: fd(fd), lastRequestTime(std::chrono::steady_clock::now()), requestCount(0),
	areHeaderComplete(false), isBodyComplete(false) { }

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
		parseHeaders(server);
	}
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
		handleGetRequest(server);

	else if (request.getMethod() == "POST")
		handlePostRequest(server);
	else
		server.handleInvalidRequest(fd, 501, "Method not implemented");
}

void	ClientState::handleGetRequest(Server &server)
{

	if (!requestBody.empty() || request.getHeader("Content-Length") != "none" || request.getHeader("Transfer-Encoding") == "chunked")
		server.handleInvalidGetRequest(fd);
	else
		server.processGetRequest(fd, request);
}



void	ClientState::handlePostRequest(Server &server)
{
	if (request.getStatus() != 200)
	{
		server.handleInvalidRequest(fd, request.getStatus());
		return;
	}
	if (request.getHeader("Transfer-Encoding") == "chunked")
	{
		server.handleInvalidRequest(fd, 501, "Chunked Transfer-Encoding Not Supported for Client Requests");
		return;
	}
	else
	{
		requestBodySize = std::stoll(request.getHeader("Content-Length"));
		if (requestBodySize > server._config.clientMaxBodySize)
		{
			std::cerr << "Body of post request is grater than the client max body size" << std::endl;
			std::cerr << "Body size: " << requestBodySize << std::endl;
			std::cerr << "Client max body size: " << server._config.clientMaxBodySize << std::endl;
			server.handleInvalidRequest(fd, 413);
			return;
		}
		initializeBodyStorage(server);
	}
}

void	ClientState::initializeBodyStorage(Server &server)
{
	std::string filename = "post_body_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + "_" + std::to_string(fd) + ".tmp";
	requestBodyFilePath = TEMP_FILE_DIRECTORY + filename;

	requestBodyFile.open(requestBodyFilePath.c_str(), std::ios::out | std::ios::binary);
	if (!requestBodyFile.is_open())
	{
		server.handleInvalidRequest(fd, 500, "Internal Server Error: Temporary File Creation Failed");
		return;
	}
	if (requestBody.size() == requestBodySize)
	{
		std::cerr << "Body of post request is completed from the first read" << std::endl;
		requestBodyFile << requestBody;
		requestBodyFile.close();
		isBodyComplete = true;
		server.processPostRequest(fd, request);
	}
	else if (requestBody.size() > requestBodySize)
	{
		std::cerr << "Body of post request is grater than the content length from the first read" << std::endl;
		requestBodyFile.close();
		server.handleInvalidRequest(fd, 400, "Request Body Exceeds Content-Length");
	}
}

void	ClientState::processBody(Server &server, const char *buffer, size_t bytesRead)
{
	std::cerr << "Processing body of post request" << std::endl;
	size_t remainingBodySize = requestBodySize - requestBodyFile.tellp();
	if (bytesRead > remainingBodySize)
	{
		std::cerr << "Body of post request is grater than the content length" << std::endl;
		requestBodyFile.write(buffer, remainingBodySize);
		requestBodyFile.close();
		isBodyComplete = true;

		server.processPostRequest(fd, request, true);
		server.removeClient(fd);
	}
	else if (bytesRead == remainingBodySize)
	{
		std::cerr << "Body of post request is completed" << std::endl;
		requestBodyFile.write(buffer, bytesRead);
		requestBodyFile.close();
		isBodyComplete = true;
		
		server.processPostRequest(fd, request);
	}
	else
	{
		std::cerr << "Adding a new buffer to the body of post request" << std::endl;
		requestBodyFile.write(buffer, bytesRead);
	}
}

int		ClientState::getFd() const
{
	return fd;
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