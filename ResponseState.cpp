#include "ResponseState.hpp"

ResponseState::ResponseState(const std::string &smallResponse, bool closeConnection)
	: type(SMALL_RESPONSE), smallResponse(smallResponse), closeConnection(closeConnection), bytesSent(0) {}

ResponseState::ResponseState(const std::string &responseHeaders, const std::string &filePath, size_t fileSize)
	: type(LARGE_RESPONSE), headers(responseHeaders), filePath(filePath), fileSize(fileSize), bytesSent(0), headersSent(0), isHeaderSent(false), currentChunkPosition(0)
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
		if (currentChunkPosition == 0)
		{
			char buffer[CHUNK_SIZE];
			fileStream.read(buffer, CHUNK_SIZE);
			std::string data(buffer, fileStream.gcount());

			std::stringstream ss;
			ss << std::hex << data.length();
			this->bytesSent += data.length();
			this->currentChunk = ss.str() + "\r\n" + data + "\r\n";
		}
		// return (currentChunk.substr(currentChunkPosition));
		return (currentChunk);
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