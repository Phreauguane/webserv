/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 16:17:08 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/26 13:35:33 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"
#include "Server.h"
#include "Request.h"

class Client
{
public:
	Client();
	Client(const Client&);
	Client(Server*);
	Client& operator=(const Client&);
	int getFd();
	std::string getRequest();
	bool sendMsg(const std::string&);
	bool readRequest();
	~Client();
private:
	Server *_server;
	size_t _size;
	std::string _request;
	Request *_req;
	int _fd;
};