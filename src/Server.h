/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:18:44 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/14 13:18:31 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"
#include "Location.h"

class Server
{
public:
	Server();
	Server(const Server&);
	Server(std::string&);
	in_addr getHost() const;
	int getPort() const;
	virtual ~Server();
private:
	in_addr _host;
	int _port;
	std::vector<Location> _locations;
	std::map<unsigned int, std::string> _error_pages;
};