#include "ResponseState.hpp"

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