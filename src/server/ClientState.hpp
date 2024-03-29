


#pragma once
#ifndef CLIENTSTATE_HPP
#define CLIENTSTATE_HPP

#include "../server/Server.hpp"

class ClientState
{
private:

	int													fd;
	std::string											clientIpAddr;
	int													requestCount;
	std::chrono::time_point<std::chrono::steady_clock>	lastRequestTime;

	HttpRequest											request;
	std::string											requestHeaders;
	std::string											requestBody;
	std::ofstream										requestBodyFile;
	size_t												requestBodySize;
	std::string											requestBodyFilePath;
	bool												areHeaderComplete;
	bool												isBodyComplete;
	
public:

	ClientState(int fd, const std::string &clientIpAddr);
	~ClientState();

	void	resetClientState();
	void	updateLastRequestTime();
	void	incrementRequestCount();

	void 	processIncomingData(Server &server, const char *buffer, size_t bytesRead);
	void	processHeaders(Server &server, const char *buffer, size_t bytesRead);
	void	processBody(Server &server, const char *buffer, size_t bytesRead);
	void	parseHeaders(Server &server);
	void	initializeBodyStorage(Server &server);

	void	handleGetRequest(Server &server);
	void	handleHeadRequest(Server &server);
	void	handlePostRequest(Server &server);

	int					getFd() const;
	const std::string	&getClientIpAddr() const;
	int					getRequestCount() const;
	const std::string			&getPostRequestFileName();
	bool				isTimedOut(size_t keepalive_timeout) const;
};


#endif /* CLIENTSTATE_HPP */
