/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:07:53 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/15 14:39:51 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.h"

int	main(int ac, char **av)
{
	(void) ac;
	(void) av;
	try 
	{
		Config conf("default.conf");
	}
	catch (std::string error)
	{
		std::cout << "Error: " << error << std::endl;
	}
	return 0;
}