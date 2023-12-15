
#include "parseException.hpp"

const char *ParseException::what() const throw()
{
    return ("Extra braces");
}

const char *EmptyConf::what() const throw()
{
    return ("Empty Configuration File");
}

const char *ExtraOpenBraces::what() const throw()
{
    return ("Extra { inside the configruation file");
}

const char *ExtraCloseBraces::what() const throw()
{
    return ("Extra } inside the configruation file");
}

const char *ExtraBraces::what() const throw()
{
    return ("Extra {} inside the configruation file");
}

const char *EmptyBraces::what() const throw()
{
    return ("Empty Braces");
}

const char *Semicolon::what() const throw()
{
    return ("Semicolon Error");
}

const char *WrongContextName::what() const throw()
{
    return ("Wrong Context name");
}

const char *HttpContextError::what() const throw()
{
    return ("http should be first");
}
const char *ServerContextError::what() const throw()
{
    return ("server doesn't take multiple parameters");
}

const char *LocationContextError::what() const throw()
{
    return ("location take one parameter");
}