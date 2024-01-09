

#pragma once
#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "BaseConfig.hpp"
#include "ServerConfig.hpp"
#include <set>


class LocationConfig : public BaseConfig
{
private:
	std::string				path;
	std::set<std::string>	allowedMethods;

public:
	LocationConfig();
	LocationConfig(const std::string &path, const ServerConfig &serverConfig);

	void				setAllowedMethods(const std::vector<std::string> &limitExceptValues);


	const std::string			&getPath() const;
	bool						isMethodAllowed(const std::string &method) const;


};

#endif /* LOCATIONCONFIG_HPP */