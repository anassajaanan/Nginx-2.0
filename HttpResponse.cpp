#include "HttpResponse.hpp"


HttpResponse::HttpResponse(): type(SMALL_FILE) { }


void	HttpResponse::setType(ResponseType type)
{
	this->type = type;
}

void	HttpResponse::setVersion(const std::string& version)
{
	this->version = version;
}

void	HttpResponse::setStatusCode(const std::string& statusCode)
{
	this->statusCode = statusCode;
}

void	HttpResponse::setStatusMessage(const std::string& statusMessage)
{
	this->statusMessage = statusMessage;
}

void	HttpResponse::setHeader(const std::string& key, const std::string& value)
{
	this->headers[key] = value;
}

void	HttpResponse::setBody(const std::string& body)
{
	this->body = body;
}

ResponseType	HttpResponse::getType() const
{
	return this->type;
}

std::string	HttpResponse::getVersion() const
{
	return this->version;
}

std::string	HttpResponse::getStatusCode() const
{
	return this->statusCode;
}

std::string	HttpResponse::getStatusMessage() const
{
	return this->statusMessage;
}

std::string	HttpResponse::getHeader(const std::string& key) const
{
	return this->headers.at(key);
}

std::string	HttpResponse::getBody() const
{
	return this->body;
}

std::string	HttpResponse::getStatusLine() const
{
	return this->version + " " + this->statusCode + " " + this->statusMessage;
}

std::string	HttpResponse::getHeadersAsString() const
{
	std::string headersAsString;

	std::map<std::string, std::string>::const_iterator it = headers.begin();
	while (it != headers.end())
	{
		headersAsString += it->first + ": " + it->second + "\r\n";
		it++;
	}
	return headersAsString;
}

std::string HttpResponse::buildResponse() const
{
	return this->getStatusLine() + "\r\n" + this->getHeadersAsString() + "\r\n" + this->getBody();
}

std::string HttpResponse::buildResponseHeaders() const
{
	return this->getStatusLine() + "\r\n" + this->getHeadersAsString() + "\r\n";
}


void	HttpResponse::generateStandardErrorResponse(const std::string &statusCode, const std::string &statusMessage, const std::string &title, const std::string &detail)
{
	this->setVersion("HTTP/1.1");
	this->setStatusCode(statusCode);
	this->setStatusMessage(statusMessage);
	this->setHeader("Content-Type", "text/html");
	
	std::string htmlBody = "<html><head><title>" + title + "</title></head>"
                           "<body><center><h1>" + statusCode + " " + statusMessage + "</h1></center>"
                           "<center>" + detail + "</center><hr><center>nginx 2.0</center></body></html>";
	this->setBody(htmlBody);
	this->setHeader("Content-Length", std::to_string(htmlBody.length()));
	this->setHeader("Connection", "close");
}

// #======================# ResponseState #======================#

ResponseState::ResponseState(const std::string &smallFileResponse, bool closeConnection)
	: type(SMALL_FILE), smallFileResponse(smallFileResponse), closeConnection(closeConnection) {}

ResponseState::ResponseState(const std::string &responseHeaders, const std::string &filePath, size_t fileSize)
	: type(LARGE_FILE), headers(responseHeaders), filePath(filePath), fileSize(fileSize), bytesSent(0), isHeaderSent(false)
{
	fileStream.open(filePath, std::ifstream::binary);
}

ResponseType ResponseState::getType() const
{
	return type;
}

const std::string &ResponseState::getSmallFileResponse() const
{
	return (smallFileResponse);
}

const std::string &ResponseState::getHeaders() const
{
	return (headers);
}

std::string ResponseState::getNextChunk()
{
	if (type == LARGE_FILE && fileStream.is_open())
	{
		char buffer[CHUNK_SIZE];

		fileStream.read(buffer, CHUNK_SIZE);
		std::streamsize bytesRead = fileStream.gcount();
		bytesSent += bytesRead;
		return std::string(buffer, bytesRead);
	}
	else
		return "";
}

bool ResponseState::isFinished() const
{
	if (type == LARGE_FILE)
		return (fileStream.eof() || bytesSent >= fileSize);
	else
		return true;
}