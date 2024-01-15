

#pragma once
#include <sys/_types/_size_t.h>
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <fstream>
#include <string>
#include <map>

#define CHUNK_SIZE 3 * 1024 // 8 KB

enum	ResponseType { SMALL_FILE, LARGE_FILE };

class HttpResponse
{

	private:
		ResponseType						type;
		std::string							version;
		std::string							statusCode;
		std::string							statusMessage;
		std::map<std::string, std::string>	headers;
		std::string							body;

		

	public:
		HttpResponse();

		std::string							filePath;
		size_t								fileSize;

		void			setType(ResponseType type);
		void			setVersion(const std::string& version);
		void			setStatusCode(const std::string& statusCode);
		void			setStatusMessage(const std::string& statusMessage);
		void			setHeader(const std::string& key, const std::string& value);
		void			setBody(const std::string& body);


		ResponseType	getType() const;
		std::string		getVersion() const;
		std::string		getStatusCode() const;
		std::string		getStatusMessage() const;
		std::string		getHeader(const std::string& key) const;
		std::string		getBody() const;


		std::string getStatusLine() const;
		std::string getHeadersAsString() const;

		std::string buildResponse() const; // for small files
		std::string	buildResponseHeaders() const; // for large files

		void	generateStandardErrorResponse(const std::string &statusCode, const std::string &statusMessage, const std::string &title, const std::string &detail);

};

class ResponseState
{

public:

	ResponseState(const std::string &smallFileResponse); // small file
	ResponseState(const std::string &responseHeaders, const std::string &filePath, size_t fileSize); // large file

	ResponseType		getType() const;

	const std::string	&getSmallFileResponse() const;
	const std::string	&getHeaders() const;

	std::string			getNextChunk();

	bool				isFinished() const;

	bool				isHeaderSent;

private:
	ResponseType	type;
	std::string		smallFileResponse;
	std::string		headers;
	std::string		filePath;
	std::ifstream	fileStream;
	size_t			fileSize;
	size_t			bytesSent;
	
};
















#endif /* HTTPRESPONSE_HPP */