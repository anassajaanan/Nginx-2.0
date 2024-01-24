
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"



void	signalHandler(int signum)
{
	std::cout << "Signal " << signum << " received" << std::endl;
	if (signum == SIGINT || signum == SIGTERM)
	{
		ServerManager::running = 0;
	}
}

int main()
{
	MimeTypeParser mimeTypeParser("mime.types");
	std::vector<ServerConfig>	serverConfigs;


    try
	{
		ConfigParser parser("./nginx.conf");
		parser.parseConfigFile();

		ConfigLoader loader(parser.getConfigTreeRoot());
		loader.loadServers(serverConfigs);

		mimeTypeParser.parseMimeTypeFile();

	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);

	Logger::init(Logger::DEBUG, "./logs/WebServer.log");

	ServerManager serverManager(serverConfigs, mimeTypeParser);
	Logger::log(Logger::DEBUG, "Starting server manager", "main");
	serverManager.start();

	Logger::cleanup();

	return 0;
}