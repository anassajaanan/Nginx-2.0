
#include "syntaxValidation.hpp"

SyntaxError::SyntaxError(const std::string &messageError) : std::runtime_error(messageError)
{
}