/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:07:53 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/17 16:42:17 by jde-meo          ###   ########.fr       */
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
		Location test("\n\
		location /cgi-bin\n\
		{\n\
		\n\
		\n\
		\n\
		root   /cgi;\n\
		autoindex ON;\n\
        index ../cgi.html;\n\
        allow_methods GET POST DELETE;\n\
        cgi_path /usr/bin/python3 /bin/bash /usr/bin/gcc;\n\
        cgi_ext .py .sh .c;\n\
		\n\
		\n\
		location /tests {\n\
			allow_methods GET;\n\
		}\n\
		alias /prout\n\
		\n\
		\n\
		location /miaou {\n\
			cgi_path miaouuu prout;\n\
			allow_methods MONSTRE DOUBLE_MONSTRE;\n\
		}\n\
		\n\
		\n\
    }");
	}
	catch (std::string error)
	{
		std::cout << "Error: " << error << std::endl;
	}
	return 0;
}