/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:32 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/23 18:26:23 by jde-meo          ###   ########.fr       */
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
		Server *srv = new Server(srv_source);
		_servers.push_back(srv);
		found = _source.find("server", found + srv_source.size());
	}
}

Config::Config()
{
	_loaded = false;
	// default constructor to do
}

Config::Config(const Config& copy)
{
	*this = copy;
}

void Config::load(const std::string& filename)
{
	Logger::log("Loading config file : " + filename, INFO);

	_source = Utils::readFile(filename); // Read full config file source
	_source = Utils::removeComments(_source); // Remove comments from source
	
	_createServers();
	
	Logger::log("Config file loaded", SUCCESS);
}

void Config::setup()
{
	_setupServers();

	Logger::log("Setup finished", SUCCESS);
}

void Config::_setupServers()
{
	try
	{
		for (size_t i = 0; i < _servers.size(); i++)
		{
			if (_servers[i])
				_servers[i]->setup();
		}
	}
	catch (const std::runtime_error& e)
	{
		Logger::log(e.what(), ERROR);
		throw std::runtime_error("Failed to setup servers");
	}
}

Config::~Config()
{
	for (size_t i = 0; i < _servers.size(); i++)
	{
		delete _servers[i];
	}
	_servers.clear();
	Logger::log("Deleted config object", DEBUG);
}

Config& Config::operator=(const Config& copy)
{
	if (this != &copy)
	{
		for (size_t i = 0; i < _servers.size(); i++)
		{
			delete _servers[i];
		}
		_servers.clear();
		
		_loaded = copy._loaded;
		_source = copy._source;
		for (size_t i = 0; i < copy._servers.size(); i++)
		{
			_servers.push_back(new Server(*(copy._servers[i])));
		}
	}
	return *this;
}