

#pragma once
#ifndef RESPONSESTATE_HPP
#define RESPONSESTATE_HPP

#include "HttpResponse.hpp"

#include <sstream>


class ResponseState
{

public:

	ResponseState(const std::string &smallResponse, bool closeConnection = false); // small response
	ResponseState(const std::string &responseHeaders, const std::string &filePath, size_t fileSize); // large response

	ResponseType		getType() const;

	const std::string	&getSmallResponse() const;
	const std::string	&getHeaders() const;

	std::string			getNextChunk();

	bool				isFinished() const;

	bool				isHeaderSent;
	bool				closeConnection;
	size_t				bytesSent;
	size_t				headersSent;

private:
	ResponseType	type;
	std::string		smallResponse;
	std::string		headers;
	std::string		filePath;
	std::ifstream	fileStream;
	size_t			fileSize;
	
	
};



#endif /* RESPONSESTATE_HPP */