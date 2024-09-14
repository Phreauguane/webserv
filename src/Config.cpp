/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:32 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/15 00:45:26 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.h"
#include "Utils.h"

std::string Config::_readFile(const std::string& filename)
{
	std::string out = "";
	std::ifstream file(filename.c_str());
	while (file)
	{
		char c;
		file.read(&c, 1);

		if (file)
			out += c;
	}
	file.close();
	return out;
}

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
	(void) copy;
	// to do
}

Config::~Config()
{
	// destructor to do
}

Config::Config(const std::string& filename)
{
	_source = _readFile(filename);
	
	std::cout << ">     Config file read      <" << std::endl;
	std::cout << ">          START            <" << std::endl;
	std::cout <<            _source              << std::endl;
	std::cout << ">           END             <" << std::endl;
	
	_source = Utils::removeComments(_source);

	std::cout << ">     Removed Comments      <" << std::endl;
	std::cout << ">          START            <" << std::endl;
	std::cout <<            _source              << std::endl;
	std::cout << ">           END             <" << std::endl;
	
	_createServers();
}