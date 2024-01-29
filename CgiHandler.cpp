#include "CgiHandler.hpp"

CgiHandler::CgiHandler(HttpRequest &request, ServerConfig &serverConfig, KqueueManager	&kq, int cgiSocket, const std::string &postPath) : cgiClientSocket(cgiSocket)
{
	this->pid = -1;
	this->postBodyFd = -1;
	this->pipeFd[0] = -1;
	this->pipeFd[1] = -1;
	this->startTime = std::chrono::steady_clock::now();
	handleCgiDirective(request, serverConfig, kq, postPath);
}

CgiHandler::~CgiHandler()
{
	if (pipeFd[0] != -1)
	{
		close(pipeFd[0]);
		pipeFd[0] = -1;
	}
	if (pipeFd[1] != -1)
	{
		close(pipeFd[1]);
		pipeFd[1] = -1;
	}
	if (postBodyFd != -1)
	{
		close(postBodyFd);
		postBodyFd = -1;
	}
}

std::string	CgiHandler::buildCgiResponse()
{
	HttpResponse response;

	response.setVersion("HTTP/1.1");
	response.setStatusCode(std::to_string(200));
	response.setStatusMessage("OK");
	response.setBody(this->cgiResponseMessage);
	// /*1*/ response.setHeader("Content-Length", std::to_string(response.getBody().length()));
	// // if (message.find("Content-Type") == std::string::npos)
	// /*2*/ response.setHeader("Content-Type", "text/plain");
	response.setHeader("Content-Length", std::to_string(response.getBody().length()));
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Server", "Nginx 2.0");
	response.setHeader("Connection", "keep-alive");

	return (response.buildResponse());
}

void	CgiHandler::addCgiResponseMessage(const std::string &cgiOutput)
{
	this->cgiResponseMessage += cgiOutput;
}

void	CgiHandler::delete2dArray(char **str)
{
	for (int i = 0; str[i]; i++)
		delete str[i];
	delete [] str;

}

char		**CgiHandler::initiateEnvVariables(HttpRequest &request, ServerConfig &serverConfig)
{
	std::string	fullQuery;
	std::vector<std::string>	envVector;

	if (!request.getQueries().empty())
		envVector.insert(envVector.end(), request.getQueries().begin(), request.getQueries().end());
	envVector.push_back("CONTENT_TYPE=" + request.getHeader("content-type"));
	envVector.push_back("CONTENT_LENGTH=" + request.getHeader("content-length"));
	envVector.push_back("HTTP_COOKIE=" + request.getHeader("Cookie"));
	for (size_t i = 0; i < request.getQueries().size(); i++)
	{
		if (i != 0)
			fullQuery += "&" + request.getQueries()[i];
		else
			fullQuery += request.getQueries()[i];
	}
	envVector.push_back("QUERY_STRING=" + fullQuery);
	envVector.push_back("HTTP_USER_AGENT= \"" + request.getHeader("user-agent") + "\"");
	envVector.push_back("REQUEST_METHOD=" + request.getMethod());
	envVector.push_back("SERVER_NAME=\"Nginx 2.0\"");
	envVector.push_back("SERVER_PROTOCOL=" + request.getVersion());
	envVector.push_back("SCRIPT_FILENAME=" + serverConfig.root + request.getUri());
	envVector.push_back("SERVER_NAME=" + request.getHeader("host"));
	//ADD PATH_INFO
	char	**envArray = new char *[envVector.size() + 1];
	size_t		counter = 0;
	for (; counter < envVector.size(); counter++)
		envArray[counter] = strdup(envVector[counter].c_str());
	envArray[counter] = NULL;
	return (envArray);
}

// we are closing in the destructor
// void	CgiHandler::closeCgiPipe()
// {
// 	if (this->pipeFd[0] > 0)
// 		close(pipeFd[0]);
// 	if (this->pipeFd[1] > 0)
// 		close(pipeFd[1]);
// }

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
			// std::cerr << "Failed To Execute" << std::endl;
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
}

int		CgiHandler::getChildPid()
{
	return (this->pid);
}

int		CgiHandler::getCgiClientSocket() const
{
	return (this->cgiClientSocket);
}

int		CgiHandler::getCgiReadFd()const
{
	return (this->pipeFd[0]);
}

const std::string	&CgiHandler::getCgiResponseMessage() const
{
	return (this->cgiResponseMessage);
}


bool	CgiHandler::isTimedOut(size_t timeout) const
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::seconds>(now - startTime) > std::chrono::seconds(timeout))
		return true;
	return false;
}

bool	CgiHandler::fileExists(const std::string &path)
{
	struct stat fileStat;

	if (stat(path.c_str(), &fileStat) == 0)
		return (true);
	return (false);
}

bool	CgiHandler::validCgiRequest(HttpRequest &request, ServerConfig &config)
{
	if (((config.root).find("/cgi-bin") == std::string::npos && (config.root + request.getUri()).find("/cgi-bin") == std::string::npos)
	|| !fileExists(config.root + request.getUri()) || !validateFileExtension(request, config))
		return false;
	return true;
}

bool	CgiHandler::validateFileExtension(HttpRequest &request, ServerConfig &config)
{
	std::vector<std::string>	cgiExten = config.cgiExtension.getExtensions();
	std::string					uri = request.getUri();

	if (uri.find('.') == std::string::npos ||
	std::find(cgiExten.begin(), cgiExten.end(),
	uri.substr(uri.find('.'), uri.length())) == cgiExten.end())
		return false;
	return true;
}
