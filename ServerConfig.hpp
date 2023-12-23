


#pragma once
#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "BaseConfig.hpp"

// Default configuration values
#define DEFAULT_SERVER_PORT 80
#define DEFAULT_SERVER_IP "0.0.0.0"
#define DEFAULT_SERVER_ROOT "/var/www/html"
#define DEFAULT_SERVER_INDEX "index.html"
#define DEFAULT_SERVER_NAME ""
#define DEFAULT_SERVER_AUTOINDEX "off"
#define DEFAULT_CLIENT_MAX_BODY_SIZE 1048576  // 1MB

class LocationConfig;

class ServerConfig : public BaseConfig
{
private:
	bool					isValidPort(const std::string &port);
	bool					isValidIPv4();

public:

	int										port;
	std::string								ipAddress;
	std::string								serverName;
	std::map<std::string, LocationConfig>	locations;

	ServerConfig();
	ServerConfig(const std::string &rootValue, const std::string &indexValue,
				const std::string &autoindexValue, const std::string &client_max_body_size,
				const std::vector<DirectiveNode *> &errorPagesDirectives);

	// setters
	void					setDefaultValues();
	void					setListen(const std::string &listenValue);
	void					setServerName(const std::string &serverNameValue);

	void					addLocation(ContextNode *locationNode);
};




#endif /* SERVERCONFIG_HPP */