#include "HttpResponse.hpp"
#include <fstream>
#include <ios>
#include <string>
#include <sys/_types/_size_t.h>


HttpResponse::HttpResponse() { }


void HttpResponse::setVersion(const std::string& version)
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

// #======================# ResponseState #======================#

ResponseState::ResponseState(const std::string &smallFileResponse)
	: type(SMALL_FILE), smallFileResponse(smallFileResponse) { }

ResponseState::ResponseState(const std::string &responseHeaders, const std::string &filePath, size_t fileSize)
	: type(LARGE_FILE), headers(responseHeaders), filePath(filePath), fileSize(fileSize), bytesSent(0)
{
	fileStream.open(filePath, std::ifstream::binary);
}

ResponseState::ResponseType ResponseState::getType() const
{
	return type;
}

const std::string &ResponseState::getSmallFileResponse() const
{
	return (smallFileResponse);
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