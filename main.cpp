
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"
#include "ServerManager.hpp"



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
    try
	{
		signal(SIGPIPE, SIG_IGN);
		signal(SIGINT, signalHandler);
		signal(SIGTERM, signalHandler);


		// ConfigParser parser("/goinfre/aajaanan/brew/etc/nginx/nginx.conf");
		ConfigParser parser("./nginx.conf");
		parser.parseConfigFile();

		MimeTypeParser mimeTypeParser("mime.types");
		mimeTypeParser.parseMimeTypeFile();
	
		ConfigNode *treeRoot = parser.getConfigTreeRoot();

		std::vector<ServerConfig>	serverConfigs;
		
		ConfigLoader loader(treeRoot);
		loader.loadServers(serverConfigs);


		ServerManager serverManager(serverConfigs, mimeTypeParser);

		serverManager.start();

	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;

	
}