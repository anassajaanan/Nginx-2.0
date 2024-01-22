#include "HttpRequest.hpp"

HttpRequest::HttpRequest()
{
	this->status = 200;
	this->recursionDepth = 0;
}

HttpRequest::HttpRequest(const std::string &requestStr)
{
	if (requestStr.empty())
		throw (std::runtime_error("Error While Getting The Request"));
	std::cout << requestStr << std::endl;
	this->status = 200;
	this->recursionDepth = 0;
	this->requestTokenizer(requestStr);
	this->normalizeUri();
}

bool HttpRequest::requestTokenizer(const std::string &requestString)
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
		if (!line.empty())
			requestVec.push_back(line);
		tmp = tmp.substr(tmp.find("\r\n") + 2, tmp.length());
		if (tmp.empty() || tmp == "\r\n")
			break ;

	}
	if (!validateRequestLine(requestVec[0]))
		return (false);
	loadRequestContent(requestVec);
	return (true);
}

bool	HttpRequest::validateRequestLine(const std::string &requestLine)
{
	if (requestLine.empty())
		return (this->setStatus(400), false);
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
			return ( this->setStatus(400), false); //400 bad request
		if (i == 0 && !token.empty())
			this->setMethod(token);
		if (i == 1 && !token.empty())
		{
			if (!this->validateUri(token))
				return (this->setStatus(400), false);
			token = token.substr(0, token.find('?'));
			this->setUri(token);
		}
		if (i == 2 && !token.empty())
		{
			if (!this->validateVersion(token))
				return (false);
			this->setVersion(token);
		}
		if (!token.empty())
			i++;
	}
	if (i != 3)
		return (this->setStatus(400), false);
	return true;
}

void	HttpRequest::setStatus(const int statusNum)
{
	this->status = statusNum;
}

int	HttpRequest::getStatus() const
{
	return (this->status);
}

std::vector<std::string>	HttpRequest::parseQueryString(const std::string &uri)
{
	std::vector<std::string> queryVector;
	std::stringstream	ss;
	std::string			queryString;

	queryString = uri.substr(uri.find('?') + 1, uri.length());
	ss << queryString;
	while (std::getline(ss, queryString, '&'))
	{
		if (!queryString.empty())
        	queryVector.push_back((queryString));
	}
    std::vector<std::string>::iterator it = queryVector.begin();
	return (queryVector);
}

bool	HttpRequest::validateUri(const std::string &str)
{
	if (str.empty() || str.find("/") == std::string::npos)
		return (false);
	// if (str.length() >= MAX_URL_LENGTH)
	// 	return (this->setStatus(414), false);
	if (str.find('?') != std::string::npos)
		this->queries = parseQueryString(str);
	return (true);
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
	// std::cout << "inside check" << std::endl;
	for (;mapIt != this->headers.end(); mapIt++)
	{
		lowerKey.resize(mapIt->first.size());
		std::transform(mapIt->first.begin(), mapIt->first.end(), lowerKey.begin(), ::tolower);
		if (lowerKey == "host")
			return false;
	}
	return true;
}

bool	HttpRequest::searchForHost()
{
	std::map<std::string, std::string>::iterator mapIt = this->headers.begin();
	std::string	lowerKey;

	for (;mapIt != this->headers.end(); mapIt++)
	{
		lowerKey.resize(mapIt->first.size());
		std::transform(mapIt->first.begin(), mapIt->first.end(), lowerKey.begin(), ::tolower);
		if (lowerKey == "host")
			return (true);
	}
	return (false);
}

bool	HttpRequest::loadRequestContent(const std::vector<std::string> &requestVec)
{
	std::stringstream			ss;
	std::vector<std::string>	splitedTokens;
	std::string					token;
	std::string					value;
	std::string					lowerString;

	if (requestVec.empty())
		return (this->setStatus(400), false);
	std::vector<std::string>::const_iterator	it = requestVec.begin() + 1;
	for (; it != requestVec.end(); it++)
	{
		std::stringstream			ss(*it);
		value = *it;
		token = *it;
		std::getline(ss, token, ':');
		lowerString.resize(token.size());
		std::transform(token.begin(), token.end(), lowerString.begin(), ::tolower);
		// std::cout << "token = " << token << std::endl;
		if (token.find(' ') != std::string::npos)
			return ( this->setStatus(400), false);
		if (lowerString  == "host")
		{
			if (!validateHost(value))
				return (this->setStatus(400), false);
		}
		else
		{
			if (!validateValue(value))
				return ( this->setStatus(400), false);
		}
		if (lowerString == "host" && !this->checkDuplicatedHost())
			return (this->setStatus(400), false);
		this->headers.insert(std::pair<std::string, std::string>(lowerString, value));
	}
	if (!this->searchForHost())
		return (this->setStatus(400), false);
	this->setHost((this->headers.find("host"))->second);
	this->headers.insert(std::pair<std::string, std::string>("none", ""));	
	if (this->getMethod() == "POST")
		if (!validatePostRequirements())
			return (false);
	return true;
}

bool		HttpRequest::validatePostRequirements()
{
	if (this->headers.find("content-length") == this->headers.end() && (this->headers.find("transfer-encoding") == this->headers.end() || this->headers.find("transfer-encoding")->second != "chunked"))
		return (this->setStatus(411), false);
	if (this->headers.find("content-length") != this->headers.end())
	{
		if (this->getHeader("content-length").find_first_not_of("0987654321") != std::string::npos)
			return (this->setStatus(400), false);
	}
	return (true);
}

bool			HttpRequest::validateHost(std::string &hostName)
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
		return true;
	}
	value = tmp.substr(tmp.find(':') + 1, tmp.length());
	if (value.empty())
		return (false);
	ss << value;
	while (std::getline(ss, token, ' '))
	{
		if (!token.empty())
			tokens.push_back(token);
	}
	if (tokens.size() != 1 || tokens[0][0] == ':')
		return (false);
	hostName = tokens[0];
	return true;
}

bool	HttpRequest::validateValue(std::string &hostName)
{
	std::string		value;
	std::string		tmp;
	int				index = 0;

	value = hostName;
	tmp = hostName;
	if (hostName.empty())
	{
		hostName = "";
		return (true);
	}
	if (tmp.find(':') == std::string::npos && tmp.find(' ') != std::string::npos)
		return (false);
	value = tmp.substr(tmp.find(':') + 1, tmp.length());
	while (index < value.length() && value[index] == ' ')
		index++;
	value = value.substr(index, value.length());
	if (value.empty())
		return (false);
	// std::cout << "val = " << value  << " size = " << value.size() << std::endl;
	if (value.empty() || value == hostName)
		hostName = "";
	else
		hostName = value;
	return true;
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
			return (this->setStatus(505), false);
	}
	return false;
}

const std::vector<std::string>	&HttpRequest::getQueries() const
{
	return (this->queries);
}

const std::map<std::string, std::string>	&HttpRequest::getHeaders() const
{
	return (this->headers);
}

bool	HttpRequest::validateVersion(const std::string &version)
{
	if (version.empty() || version.find("/") == std::string::npos || version.find(".") == std::string::npos)
		return (this->setStatus(400), false);
	if (std::count(version.begin(), version.end(), '.') != 1)
		return (this->setStatus(400), false);
	std::stringstream	ss(version);
	std::string			token;
	int					i = 0;
	while (std::getline(ss, token, '/'))
	{
		if (i == 0 && token != "HTTP")
			return (this->setStatus(400), false);
		if (i == 1)
		{
			if (!this->checkVersionNumber(token))
				return (this->setStatus(505), false);
		}
		i++;
	}
	return true;
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
	std::string		lowerKey;

	if (key.empty())
		return (this->headers.find("none")->second);
	lowerKey.resize(key.size());
	std::transform(key.begin(), key.end(), lowerKey.begin(), ::tolower);
	if (this->headers.find(lowerKey) != this->headers.end())
		return (this->headers.find(lowerKey)->second);
	return (this->headers.find("none")->second);
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

std::string	&HttpRequest::getUri()
{
	return (this->uri);
}


HttpRequest::~HttpRequest()
{
	// write(2, "i went out Dumbass", 19);
	std::cout << "i went out" << std::endl;
}

int	HttpRequest::getRecursionDepth() const
{
	return (this->recursionDepth);
}

void	HttpRequest::incrementRecursionDepth()
{
	recursionDepth++;
}

void	HttpRequest::normalizeUri()
{
	std::string	normalizedUri;
	bool	lastWasSlash = false;

	for (size_t i = 0; i < uri.size(); i++)
	{
		if (uri[i] == '/')
		{
			if (!lastWasSlash)
			{
				normalizedUri += uri[i];
				lastWasSlash = true;
			}
		}
		else {
			normalizedUri += uri[i];
			lastWasSlash = false;
		}
	}
	uri = normalizedUri;
}