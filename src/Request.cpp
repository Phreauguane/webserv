/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/26 13:12:50 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/26 14:00:58 by jde-meo          ###   ########.fr       */
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
	parseRequest();
}

void Request::parseRequest()
{
	std::vector<std::string> head_body = Utils::splitString(request, "\n\n");
	if (head_body.size() > 1)
		body = head_body[1];
	std::cout << "header :\n" << head_body[0] << std::endl;
	std::cout << "body :\n" << body << std::endl;
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
			if (strs[0] == "User-Agent")
				user_agent = strs[1];
			if (strs[0] == "Accept")
				accept = Utils::splitString(strs[1], ",");
			if (strs[0] == "Connection")
				keep_alive = (strs[1] == "keep-alive");
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

Request::~Request()
{
	//
}