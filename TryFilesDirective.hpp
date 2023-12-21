

#pragma once
#ifndef TRYFILESDIRECTIVE_HPP
# define TRYFILESDIRECTIVE_HPP

#include <vector>
#include <string>

class TryFilesDirective
{
private:
	std::vector<std::string>	paths;
	std::string					fallBackUri;
	int							fallBackStatusCode;

public:
	TryFilesDirective();
	~TryFilesDirective();

	void					addPath(const std::string &path);
	void					setFallBackUri(const std::string &uri);
	void					setFallBackStatusCode(int code);
};









#endif /* TRYFILESDIRECTIVE_HPP */
