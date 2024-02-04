

#pragma once
#ifndef RESPONSESTATE_HPP
#define RESPONSESTATE_HPP

#include "HttpResponse.hpp"

#include <sstream>


#define CHUNK_SIZE  16384 // 16 KB

class ResponseState
{

private:
	ResponseType	type;
	std::string		smallResponse;
	std::string		headers;
	std::string		filePath;
	std::ifstream	fileStream;
	size_t			fileSize;

public:

	ResponseState(const std::string &smallResponse, bool closeConnection = false); // small response
	ResponseState(const std::string &responseHeaders, const std::string &filePath, size_t fileSize); // large response

	bool				closeConnection;
	size_t				bytesSent;
	size_t				headersSent;
	bool				isHeaderSent;
	std::string			currentChunk;
	size_t				currentChunkPosition;

	ResponseType		getType() const;
	const std::string	&getSmallResponse() const;
	const std::string	&getHeaders() const;
	std::string			getNextChunk();

	bool				isFinished() const;
};



#endif /* RESPONSESTATE_HPP */
