/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:29 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/27 14:10:50 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.h"
#include "Client.h"

class Config
{
public:
	Config();
	Config(char**);
	Config(const Config&);
	void load(const std::string&);
	void setup();
	void run(bool*);
	Config& operator=(const Config&);
	~Config();
private:
	void _createServers();
	void _setupServers();
	void _initEpoll();
	void _addFd(int, uint32_t);
	void _modFd(int, uint32_t);
	void _checkForConnections(size_t);
	void _handleRequests(size_t);
private:
	char **_env;
	int _epollfd;
	struct epoll_event _epollevents[MAX_EVENTS];
	std::string _source;
	std::vector<Server*> _servers;
	std::vector<Client*> _clients;
};