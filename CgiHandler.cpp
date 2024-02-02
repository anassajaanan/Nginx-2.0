#include "CgiHandler.hpp"
#include "Server.hpp"

CgiHandler::CgiHandler(HttpRequest &request, ServerConfig &config, EventPoller *eventManager, int clientSocket,  const std::string &postPath)
	: pid(-1), postBodyFd(-1), cgiClientSocket(clientSocket), isValid(true)
{
	pipeFd[0] = -1;
	pipeFd[1] = -1;
	this->startTime = std::chrono::steady_clock::now();
		handleCgiDirective(request, config, eventManager, postPath);
}

CgiHandler::~CgiHandler()
{
	if (pipeFd[0] != -1)
	{
		close(pipeFd[0]);
		pipeFd[0] = -1;
	}
	if (postBodyFd != -1)
	{
		close(postBodyFd);
		postBodyFd = -1;
	}
}

void	CgiHandler::addCgiResponseMessage(const std::string &cgiOutput)
{
	this->cgiResponseMessage += cgiOutput;
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
	response.setHeader("Content-Type", "text/plain");
	response.setHeader("Server", "Nginx 2.0");
	response.setHeader("Connection", "keep-alive");

	return (response.buildResponse());
}

char		**CgiHandler::initiateEnvVariables(HttpRequest &request, ServerConfig &config)
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
	envVector.push_back("SCRIPT_FILENAME=" + config.root + request.getUri());
	envVector.push_back("SERVER_NAME=" + request.getHeader("host"));
	envVector.push_back("PATH=/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/System/Cryptexes/App/usr/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/share/dotnet:~/.dotnet/tools:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/local/bin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/bin:/var/run/com.apple.security.cryptexd/codex.system/bootstrap/usr/appleinternal/bin:/opt/homebrew/bin:/opt/homebrew/sbin");
	char	**envArray = new char *[envVector.size() + 1];
	size_t		counter = 0;
	for (; counter < envVector.size(); counter++)
	{
		envArray[counter] = new char[envVector[counter].length() + 1];
		strcpy(envArray[counter], envVector[counter].c_str());
	}
	envArray[counter] = NULL;
	return (envArray);
}

void	CgiHandler::handleCgiDirective(HttpRequest &request, ServerConfig &config, EventPoller *eventManager, const std::string &postPath)
{
	char	**parameters;
	char	**envp;

	if (pipe(this->pipeFd) < 0)
	{
		Logger::log(Logger::ERROR, "Failed To Create Pipe", "CgiHandler::handleCgiDirective");
		this->isValid = false;
		return ;
	}
		
	envp = initiateEnvVariables(request, config);
	parameters = new char *[2];
	parameters[0] = new char[config.root.length() + request.getUri().length() + 1];
	strcpy(parameters[0], (config.root + request.getUri()).c_str());
	parameters[1] = NULL;

	if (request.getMethod() == "POST")
	{
		postBodyFd = open(postPath.c_str(), O_RDONLY);
		if (postBodyFd < 0)
		{
			Logger::log(Logger::ERROR, "Failed To Open Post Body File", "CgiHandler::handleCgiDirective");
			this->isValid = false;
			return ;
		}
	}

	pid = fork();
	if (pid < 0)
	{
		Logger::log(Logger::ERROR, "Failed To Fork", "CgiHandler::handleCgiDirective");
		this->isValid = false;
		return ;
	}
	else if (pid == 0)
	{
		close(this->pipeFd[0]);
		dup2(this->pipeFd[1], STDOUT_FILENO);
		close(this->pipeFd[1]);

		if (request.getMethod() == "POST")
		{
			dup2(this->postBodyFd, STDIN_FILENO);
			close(this->postBodyFd);
		}
		if (execve(parameters[0], parameters, envp) < 0)
		{
			// Logger::log(Logger::ERROR, "Failed To Execute Cgi Script", "CgiHandler::handleCgiDirective");
			this->delete2dArray(parameters);
			this->delete2dArray(envp);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		close(this->pipeFd[1]);
		int flags = fcntl(this->pipeFd[0], F_GETFL, 0);
		if (flags < 0)
		{
			Logger::log(Logger::ERROR, "Failed To Get ReadEnd Of The Pipe Flags", "CgiHandler::handleCgiDirective");
			this->isValid = false;
			return ;
		}
		if (fcntl(pipeFd[0], F_SETFL, flags | O_NONBLOCK) < 0)
		{
			Logger::log(Logger::ERROR, "Failed To Set ReadEnd Of The Pipe To Non Blocking", "CgiHandler::handleCgiDirective");
			this->isValid = false;
			return ;
		}
		eventManager->registerEvent(this->pipeFd[0], READ);
		this->delete2dArray(parameters);
		this->delete2dArray(envp);
	}
}

void	CgiHandler::delete2dArray(char **str)
{
	for (size_t i = 0; str[i]; i++)
		delete[] str[i];
	delete[] str;
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

bool	CgiHandler::isValidCgi() const
{
	return (this->isValid);
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
