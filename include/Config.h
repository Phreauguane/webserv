/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:29 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/25 18:16:17 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.h"
#include "Client.h"

class Config
{
public:
	Config();
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
	bool _loaded;
	int _epollfd;
	struct epoll_event _epollevents[MAX_EVENTS];
	std::string _source;
	std::vector<Server*> _servers;
	std::vector<Client*> _clients;
};