

#pragma once
#ifndef RESPONSESTATE_HPP
#define RESPONSESTATE_HPP

#include "HttpResponse.hpp"


class ResponseState
{

public:

	ResponseState(const std::string &smallFileResponse, bool closeConnection = false); // small file
	ResponseState(const std::string &responseHeaders, const std::string &filePath, size_t fileSize); // large file

	ResponseType		getType() const;

	const std::string	&getSmallFileResponse() const;
	const std::string	&getHeaders() const;

	std::string			getNextChunk();

	bool				isFinished() const;

	bool				isHeaderSent;
	bool				closeConnection;

private:
	ResponseType	type;
	std::string		smallFileResponse;
	std::string		headers;
	std::string		filePath;
	std::ifstream	fileStream;
	size_t			fileSize;
	size_t			bytesSent;
	
};



#endif /* RESPONSESTATE_HPP */