
#include "ConfigParser.hpp"
#include "ConfigLoader.hpp"
#include "EventPoller.hpp"
#include "MimeTypeConfig.hpp"
#include "ServerManager.hpp"
#include "Logger.hpp"



static void	signalHandler(int signum)
{
	if (signum == SIGINT || signum == SIGTERM)
	{
		ServerManager::running = 0;
	}
	else if (signum == SIGCHLD)
	{
		int status;
		pid_t pid;
		while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
		{
			
		}
	}
}

int main()
{
	std::vector<ServerConfig>	serverConfigs;
	MimeTypeConfig				mimeTypeConfig;
	EventPoller					*eventManager;

    try
	{
		ConfigParser parser("conf/nginx.conf");
		parser.parseConfigFile();

		MimeTypeParser mimeTypeParser("./conf/mime.types");
		mimeTypeParser.parseMimeTypeFile(mimeTypeConfig);

		ConfigLoader loader(parser.getConfigTreeRoot());
		loader.loadServers(serverConfigs);

		eventManager = new EventManager();
	
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}


	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;	
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGCHLD, &sa, NULL);


	Logger::init(Logger::DEBUG, "./logs/WebServer.log");

	ServerManager serverManager(serverConfigs, mimeTypeConfig);
	Logger::log(Logger::DEBUG, "Starting server manager", "main");
	serverManager.start();

	Logger::cleanup();

	delete eventManager;

	return 0;
}
