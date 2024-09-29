/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:18:44 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/29 15:21:17 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"
#include "Location.h"
#include "Request.h"
#include "Response.h"

enum req_type
{
	T_FILE,
	T_DIR
};

class Server
{
public:
	Server();
	Server(const Server&);
	Server(const std::string&, char**);
	Server& operator=(const Server&);
	in_addr_t getHost() const;
	void setup();
	int getSockFd() const;
	std::string getIp();
	Response& executeRequest(const Request&);
	size_t getMaxBodySize();
	void printDetails() const;
	~Server();
private:
	Response& _get(const Request&);
	Response& _post(const Request&);
	Response& _delete(const Request&);
	req_type _getType(const Request&);
	void _parseSource(const std::string&);
	void _setupServAddr();
	Location *_getLocation(const std::string&);
private:
	char **_env;
	std::string _name, _ip_addr;
	bool _ready;
	struct sockaddr_in _servaddr;
	in_addr_t _host;
	int _port, _sockfd;
	Location *_root_loc;
	std::map<unsigned int, std::string> _error_pages;
	unsigned int _max_body_size;
};
