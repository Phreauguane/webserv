/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:07:53 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/25 18:26:36 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include "Config.h"

bool shouldClose = false;

void sigHandler(int signum)
{
	if (signum == SIGINT)
	{
		std::cout << "\b\b";
		Logger::log("Closing server...", INFO);
		shouldClose = true;
	}
}

int	main(int ac, char **av)
{
	signal(SIGINT, sigHandler);
	// Logger::setLogFile("default.log");
	// Logger::setMinLogLevel(INFO);
	(void) ac;
	(void) av;
	Config config;
	try
	{
		config.load("default.conf");
		config.setup();
		config.run(&shouldClose);
	}
	catch (const std::runtime_error& e)
	{
		Logger::log(e.what(), ERROR);
	}
	Logger::saveLog();
	return 0;
}