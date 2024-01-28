#include "CgiHandler.hpp"

CgiHandler::CgiHandler(HttpRequest &request, ServerConfig &serverConfig, KqueueManager	&kq, int cgiSocket, const std::string &postPath) : cgiClientSocket(cgiSocket)
{
	this->startTime = std::chrono::steady_clock::now();
	handleCgiDirective(request, serverConfig, kq, postPath);
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

int		CgiHandler::getCgiReadFd()const
{
	return (this->pipeFd[0]);
}

int		CgiHandler::getCgiWriteFd()const
{
	return (this->pipeFd[1]);
}

int		CgiHandler::getCgiClientSocket() const
{
	return (this->cgiClientSocket);
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

void	CgiHandler::closeCgiPipe()
{
	if (this->pipeFd[0] > 0)
		close(pipeFd[0]);
	if (this->pipeFd[1] > 0)
		close(pipeFd[1]);
	
}

void	CgiHandler::handleCgiDirective(HttpRequest &request,  ServerConfig &serverConfig, KqueueManager	&kq, const std::string &postPath)
{
	char	**parameters;
	char	**envp;

	if (pipe(this->pipeFd) < 0)
	{
		std::cerr << "Pipe Error" << std::endl;
		return ;
	}
	
	envp = initiateEnvVariables(request, serverConfig);
	parameters = new char *[2];
	parameters[0] = strdup((serverConfig.root + request.getUri()).c_str());
	parameters[1] = NULL;
	if (request.getMethod() == "POST")
	{
		postBodyFd = open(postPath.c_str(), O_RDONLY);
		if (postBodyFd < 0)
		{
			std::cout << "Error While Opening The Body File" << std::endl;
		}
	}
	pid = fork();
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
		if (postBodyFd > 0)
			close(postBodyFd);
		if (execve(parameters[0], parameters, envp) < 0)
		{
			std::cerr << "Failed To Execute" << std::endl;
			this->delete2dArray(parameters);
			this->delete2dArray(envp);
			exit(0);
		}
	}
	else
	{
	
		close(this->pipeFd[1]);
		if (fcntl(pipeFd[0], F_SETFL, O_NONBLOCK) < 0)
		{
			std::cerr << "cant be non blocking" << std::endl;
		}
		kq.registerEvent(this->pipeFd[0], EVFILT_READ);
		if (postBodyFd > 0)
			close(postBodyFd);
	}
	this->delete2dArray(parameters);
	this->delete2dArray(envp);
	std::cout << "doe cgi" << std::endl;
}

int				CgiHandler::getChildPid()
{
	return (this->pid);
}


void			CgiHandler::setCgiResponseMessage(const std::string &messageValue)
{
	this->cgiResponseMessage = messageValue;
}

void	CgiHandler::addCgiResponseMessage(const std::string &messageValue)
{
	this->cgiResponseMessage += messageValue;
}

const std::string		&CgiHandler::getCgiResponseMessage() const
{
	return (this->cgiResponseMessage);
}

CgiHandler::~CgiHandler() { }

bool	CgiHandler::isTimedOut(size_t timeout) const
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(now - startTime) > std::chrono::seconds(timeout))
		return true;
	return false;
}
