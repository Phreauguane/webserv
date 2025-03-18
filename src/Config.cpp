#include "Config.h"
#include "Utils.h"

void Config::_createServers()
{
	size_t found = _source.find("server");
	while (found != std::string::npos)
	{
		std::string srv_source = Utils::readBrackets(_source, found);
		Server *srv = new Server(srv_source, _env);
		_servers.push_back(srv);
		found = _source.find("server", found + srv_source.size());
	}
}

Config::Config()
{
	//
}

Config::Config(char **env)
{
	_env = env;
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

	Logger::log("All servers ready", DEBUG);
}

void Config::run(bool *shouldClose)
{
	_initEpoll();

	Logger::log("Waiting for connections...", INFO);
	while (1)
	{
		int nfds = epoll_wait(_epollfd, _epollevents, MAX_EVENTS, 30000);
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
						_modFd(_clients[j]->getFd(), EPOLLIN | EPOLLOUT | EPOLLET);
					else
						disconnect = true;
				}
				else if (_epollevents[i].events & EPOLLOUT)
				{
					if (_clients[j]->sendResponse())
					{
						Logger::log("Sent response to client", INFO);
						disconnect = true;
					}
					else
					{
						Logger::log("Can't send response, Disconnecting...", ERROR);
						disconnect = true;
					}
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
		_addFd(_servers[i]->getSockFd(), EPOLLIN | EPOLLOUT | EPOLLET);
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
		for (size_t i = 0; i < _clients.size(); ++i)
		{
			delete _clients[i];
		}
		_clients.clear();

		_env = copy._env;
		_epollfd = copy._epollfd;
		std::memcpy(_epollevents, copy._epollevents, MAX_EVENTS * sizeof(epoll_event));
		_source = copy._source;
		for (size_t i = 0; i < copy._servers.size(); ++i)
		{
			_servers.push_back(new Server(*(copy._servers[i])));
		}
		for (size_t i = 0; i < copy._clients.size(); ++i)
		{
			_clients.push_back(new Client(*(copy._clients[i])));
		}
	}
	return *this;
}
