/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:19:33 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/15 18:28:49 by jde-meo          ###   ########.fr       */
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
	*this = copy;
}

void Server::_parseSource(const std::string& source)
{
	(void) source;
	// Search for 'name' 'host' 'listen' and 'root' as they are necessary
}

Server::Server(const std::string& source)
{
	_root_loc = new Location();
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

Server& Server::operator=(const Server& copy)
{
	_host = copy._host;
	_port = copy._port;
	_root_loc = new Location(*(copy._root_loc));
	_error_pages = copy._error_pages;
	return *this;
}