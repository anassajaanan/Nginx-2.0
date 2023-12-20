

#pragma once
#ifndef TRYFILESCONFIG_HPP
# define TRYFILESCONFIG_HPP

#include <vector>
#include <string>

class TryFilesConfig
{
private:
	std::vector<std::string>	paths;
	std::string					fallBackUri;
	int							fallBackStatusCode;

public:
	TryFilesConfig();
	~TryFilesConfig();

	void					addPath(const std::string &path);
	void					setFallBackUri(const std::string &uri);
	void					setFallBackStatusCode(int code);
};









#endif /* TRYFILESCONFIG_HPP */
