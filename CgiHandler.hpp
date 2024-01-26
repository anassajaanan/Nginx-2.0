
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
		int		pipeFd[2];
		std::string	cgiResponseMessage;
	public:
		CgiHandler(HttpRequest &request, ServerConfig &serverConfig, KqueueManager	&kq);
		HttpResponse			serveCgiOutput(const std::string &message);
		void					delete2dArray(char **str);
		void					setCgiResponseMessage(const std::string &messageValue);
		const std::string		&getCgiResponseMessage() const;
		char					**initiateEnvVariables(HttpRequest &request, ServerConfig &serverConfig);
		void					handleCgiDirective(HttpRequest &request,  ServerConfig &serverConfig, KqueueManager	&kq);
		int						getCgiReadFd()const;
		int						getCgiWriteFd()const;

		//utilities
		void					closeCgiPipe();
		~CgiHandler();


};


#endif /* CGI_HANDLER */
