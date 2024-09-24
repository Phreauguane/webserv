/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:29 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/24 14:58:40 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.h"

class Config
{
public:
	Config();
	Config(const Config&);
	void load(const std::string&);
	void setup();
	void run();
	Config& operator=(const Config&);
	~Config();
private:
	void _createServers();
	void _setupServers();
	void _initEpoll();
	void _addFd(int, uint32_t);
private:
	bool _loaded;
	int _epollfd;
	std::string _source;
	std::vector<Server*> _servers;
};