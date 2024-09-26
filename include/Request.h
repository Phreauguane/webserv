/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:02:10 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/26 13:58:06 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"

class Request
{
public:
	Request();
	Request(const Request&);
	Request(const std::string&);
	void parseRequest();
	void parseBody();
	Request& operator=(const Request&);
	~Request();
public:
	std::string request;
	std::string method;
	std::string path;
	std::string http_version;
	std::string host;
	std::string user_agent;
	std::vector<std::string> accept;
	bool keep_alive;
	std::string body;
};