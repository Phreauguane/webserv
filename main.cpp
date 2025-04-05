#include "webserv.h"
#include "Config.h"

bool shouldClose = false;
bool shouldRestart = true;

void sigHandler(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << "\b\b";
		Logger::log("-----------------------", INFO);
		Logger::log("     Closing server    ", INFO);
		Logger::log("-----------------------", INFO);
		shouldClose = true;
		shouldRestart = false;
	}
	if (signum == SIGTSTP)
	{
		std::cout << "\b\b";
		Logger::log("-----------------------", SUCCESS);
		Logger::log("   Restarting server   ", SUCCESS);
		Logger::log("-----------------------", SUCCESS);
		shouldClose = true;
		shouldRestart = true;
	}
}

int	main(int ac, char **av, char **env)
{
	signal(SIGINT, sigHandler);
	signal(SIGTSTP, sigHandler);
	Logger::setMinLogLevel(LOG_LEVEL);
	std::string config_file = "default.conf";
	if (ac == 2)
	{
		config_file = av[1];
		if (access(av[1], R_OK) != 0)
		{
			std::cout << "Error: Cannot read configuration file" << std::endl;
			return 1;
		}
	}
	if (ac > 2)
	{
		Logger::log("Too much arguments", ERROR);
		return -1;
	}

	Logger::log("-----------------------", SUCCESS);
	Logger::log("        WEBSERV        ", SUCCESS);
	Logger::log("  - [CTRL C]  close -  ", SUCCESS);
	Logger::log("  - [CTRL Z] reload -  ", SUCCESS);
	Logger::log("-----------------------", SUCCESS);

	while (shouldRestart)
	{
		shouldClose = false;
		try
		{
			Config config(env);
			config.load(config_file);
			config.setup();
			config.run(&shouldClose);
		}
		catch (const std::exception& e)
		{
			Logger::log(e.what(), ERROR);
			shouldRestart = false;
		}
	}
	Logger::logErrors();
	Logger::saveLog();
	return 0;
}
