#include "CgiDirective.hpp"

CgiDirective::CgiDirective() { }

void	CgiDirective::setExtensions(const std::vector<std::string> &content)
{
    if (content.empty())
        return;
    for (size_t i = 0; i < content.size();i++)
    {
        if (content[i].find('.') == std::string::npos || content[i].size() <= 2 || content[i].substr(0, 2) != "*.") // optional give a max length 
            throw (std::runtime_error("invalid cgi extension in \"cgi_extension\" directive"));
    }
    this->_isEnabled = true;
    for (size_t i = 0; i < content.size(); i++)
        this->extensions.push_back(content[i].substr(1, content[i].length()));
}

std::vector<std::string>    &CgiDirective::getExtensions()
{
    return this->extensions;
}

bool CgiDirective::isEnabled()
{
    return (this->_isEnabled);
}

CgiDirective::~CgiDirective() { }