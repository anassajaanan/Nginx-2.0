
#pragma once
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "HttpRequest.hpp"
#include "RequestHandler.hpp"
#include "ServerConfig.hpp"
#include <string>
#include "KqueueManager.hpp"

class CgiHandler
{
private:
	int 												pid;
	int													pipeFd[2];
	int													postBodyFd;
	int													cgiClientSocket;
	std::string											cgiResponseMessage;
	std::chrono::time_point<std::chrono::steady_clock>	startTime;
	
public:
	CgiHandler(HttpRequest &request, ServerConfig &serverConfig, KqueueManager	&kq, int cgiSocket,  const std::string &postPath = "");
	~CgiHandler();
	
	std::string				buildCgiResponse();
	void					delete2dArray(char **str);
	void					addCgiResponseMessage(const std::string &cgiOutput);
	char					**initiateEnvVariables(HttpRequest &request, ServerConfig &serverConfig);
	void					handleCgiDirective(HttpRequest &request,  ServerConfig &serverConfig, KqueueManager	&kq, const std::string &postPath);
	
	static bool				fileExists(const std::string &path);
	static bool				validateFileExtension(HttpRequest &request, ServerConfig &config);
	static bool				validCgiRequest(HttpRequest &request, ServerConfig &config);


	int						getChildPid();
	int						getCgiClientSocket()const;
	int						getCgiReadFd() const;
	const std::string		&getCgiResponseMessage() const;

	//utilities
	void					closeCgiPipe();


	bool					isTimedOut(size_t timeout) const;


};


#endif /* CGI_HANDLER */
