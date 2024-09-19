/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:07:53 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/19 19:37:35 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.h"

int	main(int ac, char **av)
{
	Logger::setLogFile("default.log");
	// Logger::setMinLogLevel(INFO);
	(void) ac;
	(void) av;

	Logger::log("Program started", INFO);
	try
	{
		Config conf("default.conf");
	}
	catch (std::string error)
	{
		Logger::log(error, ERROR);
	}
	Logger::saveLog();
	return 0;
}