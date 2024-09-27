/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:07:53 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/27 14:16:23 by jde-meo          ###   ########.fr       */
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

int	main(int ac, char **av, char **env)
{
	signal(SIGINT, sigHandler);
	std::string config_file = "default.conf";
	if (ac == 2)
		config_file = av[1];
	if (ac > 2)
	{
		Logger::log("Too much arguments", ERROR);
		return -1;
	}
	
	try
	{
		Config config(env);
		config.load(config_file);
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