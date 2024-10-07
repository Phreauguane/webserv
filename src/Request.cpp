/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmidou <rmidou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:12:50 by jde-meo           #+#    #+#             */
/*   Updated: 2024/10/07 16:00:50 by rmidou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.h"
#include "Utils.h"

Request::Request()
{
	//
};

Request::Request(const Request& copy)
{
	*this = copy;
}

Request::Request(const std::string& req)
{
	request = req;
	Logger::log("Received request", DEBUG);
	Logger::log(request, TEXT);
	parseRequest();
}

void Request::parseRequest()
{
	std::vector<std::string> head_body = Utils::splitString(request, "\r\n\r\n");
	if (head_body.size() > 1)
		body = head_body[1];
	std::vector<std::string> lines = Utils::splitString(head_body[0], "\n");
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (i == 0)
		{
			std::vector<std::string> strs = Utils::splitString(lines[i], " ");
			if (strs.size() != 3)
				throw std::runtime_error("Invalid Header : " + lines[i]);
			method = strs[0];
			path = strs[1];
			http_version = strs[2];
		}
		else
		{
			std::vector<std::string> strs = Utils::splitString(lines[i], ": ");
			if (strs[0] == "host")
				host = strs[1];
			else if (strs[0] == "Accept")
				accept = Utils::splitString(strs[1], ",");
			else if (strs[0] == "Connection")
				keep_alive = (strs[1] == "keep-alive");
			else
				attributes[strs[0]] = strs[1];
		}
	}
}

Request& Request::operator=(const Request& copy)
{
	if (this == &copy)
		return *this;
	request = copy.request;
	method = copy.method;
	path = copy.path;
	http_version = copy.http_version;
	host = copy.host;
	accept = copy.accept;
	keep_alive = copy.keep_alive;
	body = copy.body;
	return *this;
}

void Request::logData()
{
	Logger::log("method = " + method, TEXT);
	Logger::log("path = " + path, TEXT);
	Logger::log("http_version = " + http_version, TEXT);
	std::cout << std::endl;
	Logger::log("host = " + host, TEXT);
	Logger::log("user_agent = " + user_agent, TEXT);
	if (keep_alive)
		Logger::log("keep_alive = TRUE", TEXT);
	else
		Logger::log("keep_alive = FALSE", TEXT);
	Logger::log("body = \\\n" + body, TEXT);
}

Request::~Request()
{
	//
}