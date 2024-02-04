


#pragma once
#ifndef RETURNDIRECTIVE_HPP
# define RETURNDIRECTIVE_HPP


#include <string>

class ReturnDirective
{
private:
	int			statusCode;
	std::string	responseTextOrUrl;
	bool		_isEnabled;

public:
	ReturnDirective();
	~ReturnDirective();

	void	setStatusCode(int statusCodeValue);
	void	setResponseTextOrUrl(const std::string &responseTextOrUrlValue);
	

	int					getStatusCode() const;
	const std::string	&getResponseTextOrUrl() const;
	bool				isEnabled() const;

	
};


#endif /* RETURNDIRECTIVE_HPP */

