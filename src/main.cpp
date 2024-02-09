
#include "parsing/ConfigParser.hpp"
#include "parsing/ConfigLoader.hpp"
#include "event_polling/EventPoller.hpp"
#include "config/MimeTypeConfig.hpp"
#include "server/ServerManager.hpp"
#include "logging/Logger.hpp"

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
		while ((pid = waitpid(-1, &status, WNOHANG)) > 0) { }
	}
}

static void	setupSignalHandlers()
{
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
}

static void	initializeConfigFile(int argc, char **argv, std::string &configFile)
{
	if (argc == 1)
	{
		configFile = "conf/nginx.conf";
	}
	else if (argc == 2)
	{
		configFile = argv[1];
	}
	else
	{
		std::cerr << "Usage: " << argv[0] << " [config file]" << std::endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv)
{
	std::vector<ServerConfig>	serverConfigs;
	MimeTypeConfig				mimeTypeConfig;
	EventPoller					*eventManager;
	std::string					configFile;

	initializeConfigFile(argc, argv, configFile);

    try
	{
		ConfigParser parser(configFile);
		parser.parseConfigFile();

		MimeTypeParser mimeTypeParser("conf/mime.types");
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

	setupSignalHandlers();

	Logger::init(Logger::DEBUG, "logs/WebServer.log");

	ServerManager serverManager(serverConfigs, eventManager, mimeTypeConfig);
	Logger::log(Logger::DEBUG, "Starting server manager", "main");
	serverManager.start();

	Logger::cleanup();
	delete eventManager;

	return 0;
}
