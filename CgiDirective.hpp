
#pragma once
#ifndef CGIDIRECTIVE_HPP
# define CGIDIRECTIVE_HPP


#include <vector>
#include <iostream>

class CgiDirective
{
    private:
        std::vector<std::string>    extensions;
        bool                        _isEnabled;

    public:
        CgiDirective();
        ~CgiDirective();

        bool    isEnabled();
        void	setExtensions(const std::vector<std::string> &content);
        std::vector<std::string>    &getExtensions();
};


#endif /* CGIDIRECTIVE_HPP */