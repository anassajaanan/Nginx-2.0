
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
    return ("Extra { in the configruation file");
}

const char *ExtraCloseBraces::what() const throw()
{
    return ("Extra } in the configruation file");
}