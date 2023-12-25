#include "Server.hpp"
#include "ServerConfig.hpp"


Server::Server(ServerConfig &config, int kq) : config(config)
{
	this->kq = kq;
}