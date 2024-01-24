#include "CgiDirective.hpp"

CgiDirective::CgiDirective() { }

void    CgiDirective::addCgiExtension(const std::string &extension)
{
    if (!extension.empty())
        this->extensions.push_back(extension);
    this->_isEnabled = true;
}

const std::vector<std::string>    &CgiDirective::getExtensions()
{
    return this->extensions;
}

bool CgiDirective::isEnabled()
{
    return (this->_isEnabled);
}

CgiDirective::~CgiDirective() { }