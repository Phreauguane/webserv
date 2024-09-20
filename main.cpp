/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:07:53 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/20 18:15:44 by jde-meo          ###   ########.fr       */
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

	try
	{
		Config conf("default.conf");
	}
	catch (std::string error)
	{
		Logger::log(error, ERROR);
	}
	Logger::log("Program ended", INFO);
	Logger::saveLog();
	return 0;
}