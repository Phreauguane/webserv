/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:02:10 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/29 14:44:51 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"

enum methods
{
	GET,
	POST,
	DELETE,
	UNKNOWN
};

class Request
{
public:
	Request();
	Request(const Request&);
	Request(const std::string&);
	void parseRequest();
	void parseBody();
	void logData();
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
	std::map<std::string, std::string> attributes;
	bool keep_alive;
	std::string body;
};