#include "RewriteDirective.hpp"


RewriteDirective::RewriteDirective() { }


RewriteDirective::RewriteDirective(const std::string &pattern, const std::string &substitution)
	: pattern(pattern), substitution(substitution) { }


RewriteDirective::~RewriteDirective() { }


void	RewriteDirective::setPattern(const std::string &pattern)
{
	this->pattern = pattern;
}

void	RewriteDirective::setSubstitution(const std::string &substitution)
{
	this->substitution = substitution;
}