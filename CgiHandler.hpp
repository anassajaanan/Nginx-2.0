
#pragma once
#include "EventPoller.hpp"
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "RequestHandler.hpp"
#include "ServerConfig.hpp"



class CgiHandler
{
private:
	int 												pid;
	int													pipeFd[2];
	int													postBodyFd;
	int													cgiClientSocket;
	std::string											cgiResponseMessage;
	std::chrono::time_point<std::chrono::steady_clock>	startTime;

	bool												isValid;
	
public:
	CgiHandler(HttpRequest &request, ServerConfig &config, EventPoller *eventManager, int clientSocket,  const std::string &postPath = "");
	~CgiHandler();
	
	std::string				buildCgiResponse();
	void					addCgiResponseMessage(const std::string &cgiOutput);
	char					**initiateEnvVariables(HttpRequest &request, ServerConfig &serverConfig);
	void					handleCgiDirective(HttpRequest &request,  ServerConfig &serverConfig, EventPoller *eventManager, const std::string &postPath);
	void					delete2dArray(char **str);


	int						getChildPid();
	int						getCgiClientSocket()const;
	int						getCgiReadFd() const;
	const std::string		&getCgiResponseMessage() const;

	//utilities
	// void					closeCgiPipe();

	bool					isValidCgi() const;
	bool					isTimedOut(size_t timeout) const;

	static bool				fileExists(const std::string &path);
	static bool				validateFileExtension(HttpRequest &request, ServerConfig &config);
	static bool				validCgiRequest(HttpRequest &request, ServerConfig &config);
};


#endif /* CGI_HANDLER */
