/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:18:44 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/25 16:29:57 by jde-meo          ###   ########.fr       */
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
	Server(const std::string&);
	Server& operator=(const Server&);
	in_addr_t getHost() const;
	void setup();
	void terminate();
	int getSockFd() const;
	std::string getIp();
	size_t getMaxBodySize();
	void printDetails() const;
	~Server();
private:
	void _parseSource(const std::string&);
	void _setupServAddr();
private:
	std::string _name, _ip_addr;
	bool _ready;
	struct sockaddr_in _servaddr;
	in_addr_t _host;
	int _port, _sockfd;
	Location *_root_loc;
	std::map<unsigned int, std::string> _error_pages;
	unsigned int _max_body_size;
};
