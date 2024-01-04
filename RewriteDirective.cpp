#include "RewriteDirective.hpp"


RewriteDirective::RewriteDirective() : _isEnabled(false) { }


RewriteDirective::RewriteDirective(const std::string &pattern, const std::string &substitution)
	: pattern(pattern), substitution(substitution), _isEnabled(true) { }


RewriteDirective::~RewriteDirective() { }


void	RewriteDirective::setPattern(const std::string &pattern)
{
	_isEnabled = true;
	this->pattern = pattern;
}

void	RewriteDirective::setSubstitution(const std::string &substitution)
{
	_isEnabled = true;
	this->substitution = substitution;
}

bool	RewriteDirective::isEnabled() const
{
	return (_isEnabled);
}