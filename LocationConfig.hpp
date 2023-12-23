

#pragma once
#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

#include "BaseConfig.hpp"
#include "ServerConfig.hpp"


class LocationConfig : public BaseConfig
{

public:
	LocationConfig();
	LocationConfig(const ServerConfig &serverConfig);
};

#endif /* LOCATIONCONFIG_HPP */