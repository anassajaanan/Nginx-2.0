

#pragma once
#ifndef REWRITEDIRECTIVE_HPP
# define REWRITEDIRECTIVE_HPP

#include <string>
#include <vector>

class RewriteDirective
{
private:
	std::string	pattern;
	std::string	substitution;

public:
	RewriteDirective(const std::string &pattern, const std::string &substitution);
	~RewriteDirective();

	void		setPattern(const std::string &pattern);
	void		setSubstitution(const std::string &substitution);

};




#endif /* REWRITEDIRECTIVE_HPP */