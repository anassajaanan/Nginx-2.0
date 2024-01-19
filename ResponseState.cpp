#include "ResponseState.hpp"

ResponseState::ResponseState(const std::string &smallResponse, bool closeConnection)
	: type(SMALL_RESPONSE), smallResponse(smallResponse), closeConnection(closeConnection), bytesSent(0) {}

ResponseState::ResponseState(const std::string &responseHeaders, const std::string &filePath, size_t fileSize)
	: type(LARGE_RESPONSE), headers(responseHeaders), filePath(filePath), fileSize(fileSize), bytesSent(0), isHeaderSent(false)
{
	fileStream.open(filePath, std::ifstream::binary);
}

ResponseType ResponseState::getType() const
{
	return type;
}

const std::string &ResponseState::getSmallResponse() const
{
	return (smallResponse);
}

const std::string &ResponseState::getHeaders() const
{
	return (headers);
}

std::string ResponseState::getNextChunk()
{
	if (type == LARGE_RESPONSE && fileStream.is_open())
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
	if (type == LARGE_RESPONSE)
		return (fileStream.eof() || bytesSent >= fileSize);
	else
		return (bytesSent >= smallResponse.size());
}