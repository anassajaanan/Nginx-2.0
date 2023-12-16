

#pragma once
#ifndef CONFIGTOKENIZER_HPP
# define CONFIGTOKENIZER_HPP


#include <string>
#include <vector>
#include <set>


class	ConfigTokenizer
{
public:
	static void	tokenize(const std::string &input, std::vector<std::string> &tokens);
};



#endif // CONFIGTOKENIZER_HPP