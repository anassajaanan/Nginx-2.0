
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "HttpRequest.hpp"
#include "RequestHandler.hpp"
#include "ServerConfig.hpp"
#include <string>

class CgiHandler
{
	private:
		std::string	cgiResponseMessage;
	public:
		CgiHandler(HttpRequest &request, ServerConfig &serverConfig);
		HttpResponse			serveCgiOutput(HttpRequest &request);
		void					delete2dArray(char **str);
		void					setCgiResponseMessage(const std::string &messageValue);
		const std::string		&getCgiResponseMessage() const;
		char					**initiateEnvVariables(HttpRequest &request, ServerConfig &serverConfig);
		std::string				handleCgiDirective(HttpRequest &request,  ServerConfig &serverConfig);
		~CgiHandler();


};


#endif /* CGI_HANDLER */
