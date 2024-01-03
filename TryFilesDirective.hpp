

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
	bool						_isEnabled;

public:
	TryFilesDirective();
	~TryFilesDirective();

	void					addPath(const std::string &path);
	void					setFallBackUri(const std::string &uri);
	void					setFallBackStatusCode(int code);
	bool					isEnabled() const;

	const std::vector<std::string>	&getPaths() const;
};


#endif /* TRYFILESDIRECTIVE_HPP */
