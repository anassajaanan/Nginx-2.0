
#ifndef PARSEEXCEPTION_HPP
#define PARSEEXCEPTION_HPP


#include "ConfigNode.hpp"
#include <exception>

class ParseException : public std::exception
{
    public:
        const char *what() const throw();
};

class EmptyConf : public std::exception
{
    public:
        const char *what() const throw();
};

class ExtraOpenBraces : public std::exception
{
    public:
        const char *what() const throw();
};

class ExtraCloseBraces : public std::exception
{
    public:
        const char *what() const throw();
};

class EmptyBraces : public std::exception
{
    public:
        const char *what() const throw();
};

class Semicolon : public std::exception
{
    public:
        const char *what() const throw();
};

#endif /* PARSEEXCEPTION_HPP */