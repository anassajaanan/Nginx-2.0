#include "HttpResponse.hpp"


HttpResponse::HttpResponse(): type(SMALL_RESPONSE) { }


void	HttpResponse::setType(ResponseType typeValue)
{
	this->type = typeValue;
}

void	HttpResponse::setVersion(const std::string& versionValue)
{
	this->version = versionValue;
}

void	HttpResponse::setStatusCode(const std::string& statusCodeValue)
{
	this->statusCode = statusCodeValue;
}

void	HttpResponse::setStatusMessage(const std::string& statusMessageValue)
{
	this->statusMessage = statusMessageValue;
}

void	HttpResponse::setHeader(const std::string& key, const std::string& value)
{
	this->headers[key] = value;
}

void	HttpResponse::setBody(const std::string& bodyValue)
{
	this->body = bodyValue;
}

void	HttpResponse::setFilePath(const std::string& filePathValue)
{
	this->filePath = filePathValue;
}

void	HttpResponse::setFileSize(size_t fileSizeValue)
{
	this->fileSize = fileSizeValue;
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

std::string	HttpResponse::getFilePath() const
{
	return this->filePath;
}

size_t	HttpResponse::getFileSize() const
{
	return this->fileSize;
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


void	HttpResponse::generateStandardErrorResponse(const std::string &statusCodeValue, const std::string &statusMessageValue, const std::string &title, const std::string &detail)
{
	this->setVersion("HTTP/1.1");
	this->setStatusCode(statusCodeValue);
	this->setStatusMessage(statusMessageValue);
	this->setHeader("Content-Type", "text/html");
	
	std::string htmlBody = "<html><head><title>" + title + "</title></head>"
                           "<body><center><h1>" + statusCodeValue + " " + statusMessageValue + "</h1></center>"
                           "<center>" + detail + "</center><hr><center>nginx 2.0</center></body></html>";
	this->setBody(htmlBody);
	this->setHeader("Content-Length", std::to_string(htmlBody.length()));
	this->setHeader("Connection", "close");
	this->setHeader("Server", "nginx 2.0");
}

