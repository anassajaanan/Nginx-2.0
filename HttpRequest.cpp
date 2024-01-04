#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const std::string &requestStr)
{
	if (requestStr.empty())
		throw (std::runtime_error("Error While Getting The Request"));
	std::cout << requestStr << std::endl;
	this->requestTokenizer(requestStr);
}

void HttpRequest::requestTokenizer(const std::string &requestString)
{
	std::string			line;
	std::string			tmp = requestString;
	std::vector<std::string>	requestVec;
	for (size_t i = 0; i < tmp.length(); i++)
	{
		line = tmp.substr(0, tmp.find("\r\n"));
		if (tmp.find("\r\n") == std::string::npos)
		{
			requestVec.push_back(line);
			break ;
		}
		if (line.empty())
			throw (std::runtime_error("Get Error"));
		requestVec.push_back(line);
		tmp = tmp.substr(tmp.find("\r\n") + 2, tmp.length());
		if (tmp.empty() || tmp == "\r\n")
			break ;

	}
	validateRequestLine(requestVec[0]);
	loadRequestContent(requestVec);
}

void	HttpRequest::validateRequestLine(const std::string &requestLine)
{
	if (requestLine.empty())
		throw (std::runtime_error("Missing HttpRequest Type"));
	std::string					token;
	std::stringstream			ss(requestLine);
	std::vector<std::string>	possibleHttpRequests;
	int							i = 0;

	possibleHttpRequests.push_back("GET");
	possibleHttpRequests.push_back("POST");
	possibleHttpRequests.push_back("DELETE");
	while (std::getline(ss, token, ' '))
	{
		if (i == 0 && std::find(possibleHttpRequests.begin(), possibleHttpRequests.end(), token) == possibleHttpRequests.end())
			throw (std::runtime_error(token + " Is Unkown HttpRequest")); //400 bad request
		if (i == 0 && !token.empty())
			this->setMethod(token);
		if (i == 1 && !token.empty())
		{
			this->validateUri(token);
			this->setUri(token);
		}
		if (i == 2 && !token.empty())
		{
			this->validateVersion(token);
			this->setVersion(token);
		}
		if (!token.empty())
			i++;
	}
	if (i != 3)
		throw (std::runtime_error("400 Bad Request"));
}

void	HttpRequest::validateUri(const std::string &str)
{
	if (str.empty() || str.find("/") == std::string::npos)
		throw (std::runtime_error("400 Bad Request"));
}

void	HttpRequest::checkArgsNumber(const std::string &arg)
{
	std::string		content;

	if (arg.empty())
		return;
	content = arg;
}

bool	HttpRequest::checkDuplicatedHost()
{
	std::map<std::string, std::string>::iterator mapIt = this->headers.begin();
	std::string	lowerKey;
	for (;mapIt != this->headers.end(); mapIt++)
	{
		lowerKey.resize(mapIt->first.size());
		std::transform(mapIt->first.begin(), mapIt->first.end(), lowerKey.begin(), ::tolower);
		if (lowerKey == "host")
			return false;
	}
	return true;
}

void	HttpRequest::searchForHost()
{
	std::map<std::string, std::string>::iterator mapIt = this->headers.begin();
	std::string	lowerKey;

	for (;mapIt != this->headers.end(); mapIt++)
	{
		lowerKey.resize(mapIt->first.size());
		std::transform(mapIt->first.begin(), mapIt->first.end(), lowerKey.begin(), ::tolower);
		if (lowerKey == "host")
			return ;
	}
	throw (std::runtime_error("400 bad Request"));
}

void	HttpRequest::loadRequestContent(const std::vector<std::string> &requestVec)
{
	std::stringstream			ss;
	std::vector<std::string>	splitedTokens;
	std::string					token;
	std::string					value;
	std::string					lowerString;

	if (requestVec.empty())
		throw (std::runtime_error("Invalid Get Contents"));
	std::vector<std::string>::const_iterator	it = requestVec.begin() + 1;
	for (; it != requestVec.end(); it++)
	{
		std::stringstream			ss(*it);
		value = *it;
		token = *it;
		std::getline(ss, token, ':');
		lowerString.resize(token.size());
		std::transform(token.begin(), token.end(), lowerString.begin(), ::tolower);
		std::cout << "token = " << token << std::endl;
		if (token.find(' ') != std::string::npos)
			throw (std::runtime_error("400 Bad Request"));
		if (lowerString  == "host")
			validateHost(value);
		else
			validateValue(value);
		if (lowerString == "host" && !this->checkDuplicatedHost())
			throw (std::runtime_error("400 Bad Request"));
		this->headers.insert(std::pair<std::string, std::string>(token, value));
	}
	this->searchForHost();
	this->setHost((this->headers.find("Host"))->second);
}

void			HttpRequest::validateHost(std::string &hostName)
{
	std::string		value;
	std::string		tmp;
	std::stringstream	ss;
	std::string	token;
	std::vector<std::string>			tokens;

	tmp = hostName;
	if (hostName.empty())
	{
		hostName = "";
		return;
	}
	value = tmp.substr(tmp.find(':') + 1, tmp.length());
	if (value.empty())
		throw (std::runtime_error("400 Bad Request"));
	ss << value;
	while (std::getline(ss, token, ' '))
	{
		if (!token.empty())
			tokens.push_back(token);
	}
	if (tokens.size() != 1 || tokens[0][0] == ':')
		throw (std::runtime_error("400 Bad Request"));
	hostName = tokens[0];
}

void	HttpRequest::validateValue(std::string &hostName)
{
	std::string		value;
	std::string		tmp;

	value = hostName;
	tmp = hostName;
	if (hostName.empty())
	{
		hostName = "";
		return;
	}
	if (tmp.find(':') == std::string::npos && tmp.find(' ') != std::string::npos)
		throw (std::runtime_error("400 Bad Request"));
	value = tmp.substr(tmp.find(':') + 1, tmp.length());
	if (value.empty() || value == hostName)
		hostName = "";
	else
		hostName = value;
}

bool	HttpRequest::checkVersionNumber(const std::string &str)
{
	if (str.length() == 3 && std::isdigit(str[0]) && str[1] == '.' && std::isdigit(str[2]))
	{
		float version = std::stof(str);
		if (version < 1)
			return false;
		if (version >= 1 && version <= 1.9)
			return true;
		else
			throw (std::runtime_error("505 HTTP Version Not Supported"));
	}
	return false;
}

void	HttpRequest::validateVersion(const std::string &version)
{
	if (version.empty() || version.find("/") == std::string::npos || version.find(".") == std::string::npos)
		throw (std::runtime_error("400 Bad Request"));
	if (std::count(version.begin(), version.end(), '.') != 1)
		throw (std::runtime_error("400 Bad Request"));
	std::stringstream	ss(version);
	std::string			token;
	int					i = 0;
	while (std::getline(ss, token, '/'))
	{
		if (i == 0 && token != "HTTP")
			throw (std::runtime_error("400 Bad Request"));
		if (i == 1)
		{
			if (!this->checkVersionNumber(token))
				throw (std::runtime_error("400 Bad Request"));
		}
		i++;

	}
}

std::vector<std::string> HttpRequest::splitByString(const std::string &str, const char *del)
{
	std::vector<std::string>	tokens;
	std::string					splitedString;
	std::string					save;

	save = str;
	for (size_t i = 0; i < save.length(); i++)
	{
		if (save.find(del) != std::string::npos)
		{
			splitedString = save.substr(0, save.find(del));
			save = save.substr(save.find(del), save.length());
		}
		else
		{
		 	splitedString = save.substr(0, save.length());
			save = "";
		}
		if (save.length() >= 2)
			save = save.substr(2, save.length());
		tokens.push_back(splitedString);
		if (save.empty() || save.length() == 1)
			break ;
	}
	if (!save.empty())
	{
		splitedString = save.substr(0, save.length());
		tokens.push_back(splitedString);
	}
	return (tokens);
}

void	HttpRequest::setHost(const std::string &hostName)
{
	this->host = hostName;
}

const std::string	&HttpRequest::getHeader(const std::string &key) const
{
	static std::string	s = "";
	if (this->headers.find(key) != this->headers.end())
		return (this->headers.find(key)->second);
	else
		return (s);
}


void	HttpRequest::setVersion(const std::string &str)
{
	this->version = str;
}

const std::string &HttpRequest::getVersion() const
{
	return (this->version);
}

const std::string	&HttpRequest::getHost() const
{
	return (this->host);
}

void	HttpRequest::setUri(const std::string &str)
{
	this->uri = str;
}

void	HttpRequest::setMethod(const std::string &str)
{
	this->method = str;
}

const std::string	&HttpRequest::getMethod() const
{
	return (this->method);
}

const std::string	&HttpRequest::getUri() const
{
	return (this->uri);
}


HttpRequest::~HttpRequest()
{
}