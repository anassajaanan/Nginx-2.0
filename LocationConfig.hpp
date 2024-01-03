

#pragma once
#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "BaseConfig.hpp"
#include "ServerConfig.hpp"


class LocationConfig : public BaseConfig
{
private:
	std::string		path;

public:
	LocationConfig();
	LocationConfig(const std::string &path, const ServerConfig &serverConfig);

	const std::string	&getPath() const;
};

#endif /* LOCATIONCONFIG_HPP */