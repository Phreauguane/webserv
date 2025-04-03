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

	_source = Utils::readFile(filename);
	_source = Utils::removeComments(_source);
	
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
        Logger::log("Calling epoll_wait...", INFO);
        int nfds = epoll_wait(_epollfd, _epollevents, MAX_EVENTS, 30000);

        if (*shouldClose)
            break;

        if (nfds < 0) {
			Logger::log("EPOLL failed", ERROR);
			continue;
		}

        Logger::log("epoll_wait returned: " + Utils::toString(nfds) + " events", INFO);
		try {
			_checkForConnections(nfds);
		} catch (const std::runtime_error &e) {
			Logger::log("Connection error: " + std::string(e.what()), ERROR);
		}
		try {
			_handleRequests(nfds);
		} catch (const std::exception &e) {
			Logger::log("Request error: " + std::string(e.what()), ERROR);
		}
		_checkForTimeouts();
    }
}

void Config::_checkForConnections(size_t nfds)
{
    for (size_t i = 0; i < nfds; i++) {
        for (size_t j = 0; j < _servers.size(); j++) {
            if (_epollevents[i].data.fd == _servers[j]->getSockFd()) {
                while (true) {
                    Client *cli = NULL;
                    try {
                        cli = new Client(_servers[j]);
                        this->_addFd(cli->getFd(), EPOLLIN | EPOLLET);
                        _clients.push_back(cli);
                        Logger::log("Client FD added to epoll: " + Utils::toString(cli->getFd()), INFO);
                    } catch (const std::runtime_error &e) {
                        if (cli == NULL)
                            break;
                        delete cli;
                        throw;
                    }
                }
            }
        }
    }
}

void Config::_disconnectClient(size_t i) {
	try {
		this->_delFd(_clients[i]->getFd());
		delete _clients[i];
		_clients.erase(_clients.begin() + i);
	} catch (const std::exception &e) {
		Logger::log("Failed to disconnect client", ERROR);
		Logger::log(e.what(), DEBUG);
	}
}

void Config::_handleRequests(size_t nfds)
{
    for (size_t i = 0; i < nfds; i++) {
        int fd = _epollevents[i].data.fd;
        uint32_t events = _epollevents[i].events;

        std::ostringstream oss;
        oss << "Event received on FD: " << fd << " - ";
        if (events & EPOLLIN) oss << "EPOLLIN ";
        if (events & EPOLLOUT) oss << "EPOLLOUT ";
        if (events & EPOLLERR) oss << "EPOLLERR ";
        if (events & EPOLLHUP) oss << "EPOLLHUP ";
        Logger::log(oss.str(), INFO);

        for (size_t j = 0; j < _clients.size();) {
            if (fd == _clients[j]->getFd()) {
                if (events & EPOLLIN) {
					ReqStatus stat = _clients[j]->readRequest();
					if (stat == FINISHED) {
						// _clients[j]->resetState();
						_clients[j]->runRequests();

						Logger::log("Request executed modifying FD for EPOLLIN | EPOLLOUT", SUCCESS);
						_modFd(_clients[j]->getFd(), EPOLLIN | EPOLLOUT);
					} else if (stat == INCOMPLETE) {
						Logger::log("Request incomplete modifying FD for EPOLLIN", DEBUG);
						_modFd(_clients[j]->getFd(), EPOLLIN);
					} else if (stat == DISCONNECT) {
						_disconnectClient(j);
						continue;
					}
				}

				if (events & EPOLLOUT) {
					if (_clients[j]->sendResponse() && _clients[j]->isIncomplete()) {
						Logger::log("Sent response modifying FD for EPOLLIN", DEBUG);
						_modFd(_clients[j]->getFd(), EPOLLIN);
					} else {
						Logger::log("Sent response disconnecting...", DEBUG);
						_disconnectClient(j);
						continue;
					}
				}

				if (events & EPOLLERR || events & EPOLLHUP) {
					_disconnectClient(j);
					continue;
				}
            }
			++j;
        }
    }
}

void Config::_checkForTimeouts() {
	size_t now = time(NULL);
	
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i]->checkTimeout(now)) {
            Logger::log("Client timeout: closing connection", INFO);
			_disconnectClient(i);
		}
	}
}

void Config::_setupServers()
{
	for (size_t i = 0; i < _servers.size();)
	{
		if (_servers[i]) {
			try {
				_servers[i]->setup();
			} catch (const std::exception &e) {
				Logger::log("Server setup: " + std::string(e.what()), ERROR);
				Logger::log("Failed to initialize server at " + _servers[i]->getIp(), ERROR);
				_servers.erase(_servers.begin() + i);
				continue;
			}
		}
		++i;
	}

	if (_servers.size() == 0)
		throw std::runtime_error("All servers' setup failed. Stopping...");
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

void Config::_delFd(int fd)
{
	if (fd < 0)
		return;
	
	struct stat statbuf;
	if (fstat(fd, &statbuf) < 0) {
		Logger::log("File descriptor " + Utils::toString(fd) + " already closed", INFO);
		return;
	}

	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw std::runtime_error("Failed to remove fd " + Utils::toString(fd) + " from epoll instance: " + strerror(errno));
}

void Config::_initEpoll()
{
	_epollfd = epoll_create(1);
	if (_epollfd < 0)
		throw std::runtime_error("Failed to create epoll instance");
	
	for (size_t i = 0; i < _servers.size();) {
		try {
			_addFd(_servers[i]->getSockFd(), EPOLLIN | EPOLLET);
		} catch (const std::exception &e) {
			Logger::log("Server init: " + std::string(e.what()), ERROR);
			Logger::log("Failed to init server at " + _servers[i]->getIp(), ERROR);
			_servers.erase(_servers.begin() + i);
			continue;
		}
		++i;
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
