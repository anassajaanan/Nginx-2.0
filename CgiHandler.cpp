#include "CgiHandler.hpp"
#include "HttpResponse.hpp"
#include "KqueueManager.hpp"
#include "ServerConfig.hpp"
#include <sys/event.h>

CgiHandler::CgiHandler(HttpRequest &request, ServerConfig &serverConfig, KqueueManager	&kq)
{
	handleCgiDirective(request, serverConfig, kq);
}

HttpResponse	CgiHandler::serveCgiOutput(const std::string &message)
{
	HttpResponse response;

	response.setVersion("HTTP/1.1");
	response.setStatusCode(std::to_string(200));
	response.setStatusMessage("OK");
	response.setBody(message);
	/*1*/ response.setHeader("Content-Length", std::to_string(response.getBody().length()));
	// if (message.find("Content-Type") == std::string::npos)
	/*2*/ response.setHeader("Content-Type", "text/plain");
	response.setHeader("Server", "Nginx 2.0");
	response.setHeader("Connection", "keep-alive");
	return (response);
}

void			CgiHandler::delete2dArray(char **str)
{
	for (int i = 0; str[i]; i++)
		delete str[i];
	delete str;
}

char		**CgiHandler::initiateEnvVariables(HttpRequest &request, ServerConfig &serverConfig)
{
	std::vector<std::string>	envVector;

	if (!request.getQueries().empty())
		envVector.insert(envVector.end(), request.getQueries().begin(), request.getQueries().end());
	envVector.push_back("CONTENT_TYPE=" + request.getHeader("content-type"));
	envVector.push_back("CONTENT_LENGHT=" + request.getHeader("content-length"));
	envVector.push_back("HTTP_COOKIE=" + request.getHeader("Cookie"));
	std::string	fullQuery;
	// int i = 0;
	for (size_t i = 0; i < request.getQueries().size(); i++)
	{
		if (i != 0)
			fullQuery += "&" + request.getQueries()[i];
		else
			fullQuery += request.getQueries()[i];
	}
	std::cout << "fall = " << fullQuery << std::endl;
	envVector.push_back("QUERY_STRING=" + fullQuery);
	std::cout << "user agent = " <<  request.getHeader("user-agent") << std::endl;
	envVector.push_back("HTTP_USER_AGENT= \"" + request.getHeader("user-agent") + "\"");
	envVector.push_back("REQUEST_METHOD=" + request.getMethod());
	envVector.push_back("SERVER_NAME=\"Nginx 2.0\"");
	envVector.push_back("SERVER_PROTOCOL=" + request.getVersion());
	envVector.push_back("SCRIPT_FILENAME=" + serverConfig.root + request.getUri());
	envVector.push_back("SERVER_NAME=" + request.getHeader("host"));
	//ADD PATH_INFO
	char	**envArray = new char * [envVector.size() + 1];
	size_t		counter = 0;
	for (; counter < envVector.size(); counter++)
		envArray[counter] = strdup(envVector[counter].c_str());
	envArray[counter] = NULL;
	return (envArray);
}

void	CgiHandler::handleCgiDirective(HttpRequest &request,  ServerConfig &serverConfig, KqueueManager	&kq)
{
	int		pid;
	char	**parameters;
	char	**envp;

	if (pipe(this->pipeFd) < 0)
	{
		std::cerr << "Pipe Error" << std::endl;
		return ;
	}
	kq.registerEvent(this->pipeFd[0], EVFILT_READ);
	envp = initiateEnvVariables(request, serverConfig);
	parameters = new char *[2];
	parameters[0] = strdup((serverConfig.root + request.getUri()).c_str());
	parameters[1] = NULL;
	pid = fork();
	int postBodyFd = open("/Users/atukka/Desktop/cursus/webserve_par/contentbody", O_RDONLY);
	if (postBodyFd < 0)
	{
		std::cout << "Error While Opening The Body File" << std::endl;
	}
	if (pid == 0)
	{
		close(this->pipeFd[0]);
		if (dup2(this->pipeFd[1], STDOUT_FILENO) < 0)
		{
			std::cerr << "Failed To Dup2 Output" << std::endl;
			this->delete2dArray(parameters);
			this->delete2dArray(envp);
			exit (0);
		}
		if (request.getMethod() == "POST")
			if (dup2(postBodyFd, STDIN_FILENO))
				std::cerr << "Failed To Dup2 Input" << std::endl;
		close(this->pipeFd[1]);
		close(postBodyFd);
		if (execve(parameters[0], parameters, envp) < 0)
		{
			std::cerr << "Failed To Execute" << std::endl;
			this->delete2dArray(parameters);
			this->delete2dArray(envp);
			exit(0);
		}
	}
	close(this->pipeFd[1]);
	if (postBodyFd > 0)
		close(postBodyFd);
	// int i = 0;
	// int status = 0;
	// std::string		toSend;
	// std::string		buf;
	// char s[1000] = {0};
	// while (read(pipeFd[0], s, 1000) > 0)
	// {
	// 	toSend += s;
	// 	std::memset(s, 0, 1000);
	// }
	// if (i < 0)
	// 	std::cerr << "Read" << std::endl;
	// std::cout << " i = " << i << std::endl;
	// std::cout << "reached = " << toSend << std::endl;
	// wait(NULL);
	// int k = 0;
	// if (setsid() < 0)
	// 	std::cout << "failed to setsid" << std::endl;
	// k = waitpid(pid, &status, WNOHANG);
	// if (k < 0)
	// 	std::cout << "Wait error" << std::endl;
	// if (WIFEXITED(status))
	// 	std::cout << "Exited" << std::endl;
	// else
	//  	std::cout << "Not Yet" << std::endl;
	// std::cout << "k = " << k << std::endl;
	// std::cout << "status = " << status << std::endl;
	// close(pipeFd[0]);
	this->delete2dArray(parameters);
	this->delete2dArray(envp);
	// return (toSend);
}

void					CgiHandler::setCgiResponseMessage(const std::string &messageValue)
{
	this->cgiResponseMessage = messageValue;
}

const std::string		&CgiHandler::getCgiResponseMessage() const
{
	return (this->cgiResponseMessage);
}

CgiHandler::~CgiHandler()
{
	std::cout << "out" << std::endl;
}
