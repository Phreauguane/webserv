/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:32 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/25 18:25:07 by jde-meo          ###   ########.fr       */
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

	Logger::log("Server ready", SUCCESS);
}

void Config::run(bool *shouldClose)
{
	_initEpoll();

	Logger::log("Waiting for connections...", INFO);
	while (1)
	{
		int nfds = epoll_wait(_epollfd, _epollevents, MAX_EVENTS, -1);
		if (*shouldClose)
			break;
		if (nfds < 0)
			throw std::runtime_error("epoll_wait error");
		_checkForConnections(nfds);
		_handleRequests(nfds);
	}
}

void Config::_checkForConnections(size_t nfds)
{
	for (size_t i = 0; i < nfds; i++)
	{
		for (size_t j = 0; j < _servers.size(); j++)
		{
			if (_epollevents[i].data.fd == _servers[j]->getSockFd())
			{
				Client *cli = new Client(_servers[j]);
				this->_addFd(cli->getFd(), EPOLLIN | EPOLLET);
				_clients.push_back(cli);
			}
		}
	}
}

void Config::_handleRequests(size_t nfds)
{
	for (size_t i = 0; i < nfds; i++)
	{
		for (size_t j = 0; j < _clients.size(); j++)
		{
			if (_epollevents[i].data.fd == _clients[j]->getFd())
			{
				bool disconnect = false;
				if (_epollevents[i].events & EPOLLIN)
				{
					if (_clients[j]->readRequest())
						_modFd(_clients[j]->getFd(), EPOLLOUT | EPOLLET);
					else
						disconnect = true;
				}
				else if (_epollevents[i].events & EPOLLOUT)
				{
					if (_clients[j]->sendMsg("miaou"))
						Logger::log("Sent response to client", INFO);
					else
						disconnect = true;
				}
				if (disconnect)
				{
					delete _clients[j];
					_clients.erase(_clients.begin() + j);
					Logger::log("Client disconnected", INFO);
				}
			}
		}
	}
}

void Config::_setupServers()
{
	try
	{
		for (size_t i = 0; i < _servers.size(); ++i)
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

void Config::_addFd(int fd, uint32_t events)
{
	struct epoll_event event;
	event.events = events;
	event.data.fd = fd;

	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &event) < 0)
		throw std::runtime_error("Failed to add fd to epoll instance");
}

void Config::_modFd(int fd, uint32_t events)
{
	struct epoll_event event;
	event.events = events;
	event.data.fd = fd;

	if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &event) < 0)
		throw std::runtime_error("Failed to modify fd in epoll instance");
}

void Config::_initEpoll()
{
	_epollfd = epoll_create(1);
	if (_epollfd < 0)
		throw std::runtime_error("Failed to create epoll instance");
	
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		_addFd(_servers[i]->getSockFd(), EPOLLIN);
	}

	Logger::log("epoll instance created", DEBUG);
}

Config::~Config()
{
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		delete _servers[i];
	}
	_servers.clear();
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		delete _clients[i];
	}
	_clients.clear();
	Logger::log("Deleted config object", DEBUG);
}

Config& Config::operator=(const Config& copy)
{
	if (this != &copy)
	{
		for (size_t i = 0; i < _servers.size(); ++i)
		{
			delete _servers[i];
		}
		_servers.clear();
		
		_loaded = copy._loaded;
		_source = copy._source;
		for (size_t i = 0; i < copy._servers.size(); ++i)
		{
			_servers.push_back(new Server(*(copy._servers[i])));
		}
	}
	return *this;
}