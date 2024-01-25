
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"
#include "MimeTypeConfig.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"



void	signalHandler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM)
	{
		ServerManager::running = 0;
	}
}

int main()
{
	std::vector<ServerConfig>	serverConfigs;
	MimeTypeConfig				mimeTypeConfig;

    try
	{
		ConfigParser parser("./config/nginx.conf");
		parser.parseConfigFile();

		MimeTypeParser mimeTypeParser("./config/mime.types");
		mimeTypeParser.parseMimeTypeFile(mimeTypeConfig);

		ConfigLoader loader(parser.getConfigTreeRoot());
		loader.loadServers(serverConfigs);
	
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	Logger::init(Logger::ERROR, "./logs/WebServer.log");

	ServerManager serverManager(serverConfigs, mimeTypeConfig);
	Logger::log(Logger::DEBUG, "Starting server manager", "main");
	serverManager.start();

	Logger::cleanup();

	return 0;
}
