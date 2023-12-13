#include "DirectiveNode.hpp"
#include "ContextNode.hpp"
#include "ConfigNode.hpp"
#include "parseException.hpp"
#include <regex>
#include <stack>
#include <sstream>
#include <fstream>
#include <iostream>




// ContextNode		*cloneContext(std::vector<std::string>::iterator it, std::vector<std::string>	serverVector)
// {
// 	(void)serverVector;
// 	std::vector<std::string>::iterator tmp = it;
// 	if (it != serverVector.begin())
// 	{
// 		if (*--tmp == "http")
// 		{
// 			tmp = it++;
// 			std::vector<std::string>	childrens;
// 			for (tmp = it; *tmp != "{" ; tmp++)
// 			{
// 				childrens.push_back(*tmp);
// 			}
// 			std::vector<std::string>::iterator tmp = childrens.begin();
// 			childrens.pop_back();
// 			for (; tmp != childrens.end(); tmp++)
// 			{
// 				std::cout << "c = " << *tmp << std::endl;
// 			}
// 			ContextNode	*node = new ContextNode(*--it);
// 			return (node);
// 		}
// 		else
// 		{
// 			// tmp = it;
// 			std::vector<std::string>	childrens;
// 			for (tmp = it; *tmp != "{" ; tmp++)
// 			{
// 				childrens.push_back(*tmp);
// 			}
// 			std::vector<std::string>::iterator tmp = childrens.begin();
// 			for (; tmp != childrens.end(); tmp++)
// 			{
// 				std::cout << "c = " << *tmp << std::endl;
// 			}
// 			ContextNode	*node = new ContextNode(*--it);
// 			return (node);
// 			// ContextNode	*node = new ContextNode(*--it);
// 		}
// 		// std::cout << "in = " << *it << std::endl;
// 	}
// 	// std::cout << "in = " << *it << std::endl;
// 	return (NULL);
// }

// using similar concept of RPN
void		syntaxValidation(std::vector<std::string> content)
{
	std::stack<std::string>	st;
	std::vector<std::string>::iterator	it = content.begin();

	if (content.empty())
		throw (EmptyConf());
	for(; it != content.end(); it++)
	{
			std::cout << "b = " << *it << std::endl;
		if (*it == "{")
		{
			st.push(*it);
		}
		if (*it == "}")
		{
			if (st.empty()) /* when we got '}' put no '{' inside the st  */
				throw (ExtraCloseBraces());
			st.pop();
		}
	}
	if (st.size() > 0)
	{
		std::cout << "in = " << st.top() << std::endl;
		throw (ExtraOpenBraces());
	}
}

int main()
{

	try
	{
		std::ifstream	ifs("nginx.conf");
		std::vector<std::string>	serverVector;
		std::vector<std::string>::iterator it;
		// ConfigNode		head();
		// ConfigNode			*tmp;
		// ContextNode			*t;

		if (!ifs.is_open())
		{
			std::cout << "Could Not Open the file" << std::endl;
			return (1);
		}
		// std::string content;
		std::string line;
		while(std::getline(ifs, line))
		{
			std::istringstream iss(line);

			// loading inside the vector 
			for(std::string str; iss >> str;)
			{
				std::cout << "[" << str << "]" << std::endl;
				serverVector.push_back(str);
			}
		}
		ifs.close();
		syntaxValidation(serverVector);

	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return (0);
}