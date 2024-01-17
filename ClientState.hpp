


#pragma once
#ifndef CLIENTSTATE_HPP
#define CLIENTSTATE_HPP

#include "Server.hpp"

class ClientState
{
private:

	int													fd;
	std::chrono::time_point<std::chrono::steady_clock>	lastRequestTime;
	int													requestCount;
	
public:

	ClientState(int fd);
	~ClientState();

	HttpRequest		request;
	std::string		requestHeaders;
	std::string		requestBody;
	std::ofstream	requestBodyFile;
	size_t			requestBodySize;
	std::string		requestBodyFilePath;
	bool			areHeaderComplete;
	bool			isBodyComplete;


	void	resetClientState();
	void	updateLastRequestTime();
	void	incrementRequestCount();

	void 	processIncomingData(Server &server, const char *buffer, size_t bytesRead);
	void	processHeaders(Server &server, const char *buffer, size_t bytesRead);
	void	processBody(Server &server, const char *buffer, size_t bytesRead);
	void	parseHeaders(Server &server);
	void	initializeBodyStorage(Server &server);

	void	handleGetRequest(Server &server);
	void	handlePostRequest(Server &server);
	
	bool	headersCompleted(const char *buffer) const;

	int		getFd() const;
	int		getRequestCount() const;
	bool	isTimedOut(size_t keepalive_timeout) const;
};


#endif /* CLIENTSTATE_HPP */