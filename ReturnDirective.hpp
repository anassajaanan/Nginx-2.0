


#pragma once
#ifndef RETURNDIRECTIVE_HPP
# define RETURNDIRECTIVE_HPP


#include <string>

class ReturnDirective
{
private:
	int			statusCode;
	std::string	responseTextOrUrl;

public:
	ReturnDirective();
	~ReturnDirective();

	void	setStatusCode(int statusCode);
	void	setResponseTextOrUrl(const std::string &responseTextOrUrl);
	
};


#endif /* RETURNDIRECTIVE_HPP */