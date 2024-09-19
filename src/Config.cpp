/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:32 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/19 19:36:46 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.h"
#include "Utils.h"

void Config::_createServers()
{
	size_t found = _source.find("server");
	while (found != std::string::npos)
	{
		std::string srv_source = Utils::readBrackets(_source, found);
		_servers.push_back(new Server(srv_source));
		found = _source.find("server", found + srv_source.size());
	}
}

Config::Config()
{
	// default constructor to do
}

Config::Config(const Config& copy)
{
	*this = copy;
}

Config::Config(const std::string& filename)
{
	_source = Utils::readFile(filename); // Read full config file source
	_source = Utils::removeComments(_source); // Remove comments from source
	
	_createServers();

	// for (size_t i = 0; i < _servers.size(); i++)
	// {
	// 	_servers[i]->printDetails();
	// 	std::cout << std::endl;
	// }
	Logger::log("Config file loaded : " + filename, SUCCESS);
}

Config::~Config()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		delete _servers[i];
	}
	Logger::log("Deleted config object", DEBUG);
}

Config& Config::operator=(const Config& copy)
{
	_source = copy._source;
	for (size_t i = 0; i < copy._servers.size(); i++)
	{
		_servers.push_back(new Server(*(copy._servers[i])));
	}
	return *this;
}