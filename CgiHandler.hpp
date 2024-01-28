
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
		HttpResponse			serveCgiOutput(const std::string &message);
		void					delete2dArray(char **str);
		void					setCgiResponseMessage(const std::string &messageValue);
		void					addCgiResponseMessage(const std::string &messageValue);
		const std::string		&getCgiResponseMessage() const;
		char					**initiateEnvVariables(HttpRequest &request, ServerConfig &serverConfig);
		void					handleCgiDirective(HttpRequest &request,  ServerConfig &serverConfig, KqueueManager	&kq, const std::string &postPath);
		int						getCgiReadFd()const;
		int						getCgiWriteFd()const;
		int						getCgiClientSocket()const;
		int						getChildPid();
		// void					setCgiResponseMessage(const std::string &str);

		//utilities
		void					closeCgiPipe();
		~CgiHandler();


		bool		isTimedOut(size_t timeout) const;


};


#endif /* CGI_HANDLER */
