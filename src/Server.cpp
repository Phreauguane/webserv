/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:19:33 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/15 00:22:04 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.h"

// calls the constructor with an empty config as parameter
Server::Server()
{
	// to do
}

Server::Server(const Server& copy)
{
	(void) copy;
	// to do
}

Server::Server(const std::string& source)
{
	/*
		Server constructor, takes source as parameter (string containing "server {...}")
		builds a Server object based on everything specified inside
		should handle synthax and parsing errors (NEEDS TO BE BULLETPROOF)

		For now only prints the source code given as parameter

		Final constructor to be done
	*/

	// print the source code with a simple header for readability
	std::cout << "> Server constructor called <" << std::endl;
	std::cout << ">          START            <" << std::endl;
	std::cout <<             source              << std::endl;
	std::cout << ">           END             <" << std::endl;
}

Server::~Server()
{
	// to do
}