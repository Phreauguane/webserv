/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:07:53 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/23 18:12:13 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.h"

int	main(int ac, char **av)
{
	// Logger::setLogFile("default.log");
	// Logger::setMinLogLevel(INFO);
	Logger::log("Program started", INFO);
	(void) ac;
	(void) av;
	Config config;
	try
	{
		config.load("default.conf");
		config.setup();
	}
	catch (const std::runtime_error& e)
	{
		Logger::log(e.what(), ERROR);
	}
	Logger::log("Program ended", INFO);
	Logger::saveLog();
	return 0;
}