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
        Logger::log("Calling epoll_wait...", INFO);
        int nfds = epoll_wait(_epollfd, _epollevents, MAX_EVENTS, 30000);

        if (*shouldClose)
            break;

        if (nfds < 0)
            throw std::runtime_error("epoll_wait error");

        Logger::log("epoll_wait returned: " + Utils::toString(nfds) + " events", INFO);
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
                // Accepter toutes les connexions en attente
                while (true)
                {
                    Client *cli = NULL;
                    try {
                        cli = new Client(_servers[j]);
                        this->_addFd(cli->getFd(), EPOLLIN | EPOLLET);
                        _clients.push_back(cli);
                        Logger::log("Client FD added to epoll: " + Utils::toString(cli->getFd()), INFO);
                    }
                    catch (const std::runtime_error& e) {
                        // Si cli est NULL, cela signifie que l'accept a échoué
                        if (cli == NULL) {
                            break;
                        }
                        // Si c'est une autre erreur, on nettoie et on la propage
                        delete cli;
                        throw;
                    }
                }
            }
        }
    }
}

void Config::_handleRequests(size_t nfds)
{
    for (size_t i = 0; i < nfds; i++)
    {
        int fd = _epollevents[i].data.fd;
        uint32_t events = _epollevents[i].events;

        // Log received events
        std::ostringstream oss;
        oss << "Event received on FD: " << fd << " - ";
        if (events & EPOLLIN) oss << "EPOLLIN ";
        if (events & EPOLLOUT) oss << "EPOLLOUT ";
        if (events & EPOLLERR) oss << "EPOLLERR ";
        if (events & EPOLLHUP) oss << "EPOLLHUP ";
        Logger::log(oss.str(), INFO);

        for (size_t j = 0; j < _clients.size();)
        {
            if (fd == _clients[j]->getFd())
            {
                bool disconnect = false;
                if (events & EPOLLIN)
                {
                    Logger::log("Reading request...", INFO);
                    if (_clients[j]->readRequest()) {
                        // Exécuter la requête immédiatement après l'avoir lue
                        // _runRequests();
						_clients[j]->runRequests();
                        
                        Logger::log("Request read successfully, modifying FD for EPOLLOUT", INFO);
                        _modFd(fd, EPOLLIN | EPOLLOUT | EPOLLET);
                    } else {
                        Logger::log("Read failed, marking for disconnect", ERROR);
                        disconnect = true;
                    }
                }
                else if (events & EPOLLOUT)
                {
                    Logger::log("Writing response...", INFO);
                    if (_clients[j]->sendResponse())
                        Logger::log("Response sent successfully", INFO);
                    else
                        Logger::log("Send failed, marking for disconnect", ERROR);
                    disconnect = true;
                }

                if (disconnect)
                {
                    Logger::log("Closing connection", INFO);
					this->_delFd(_clients[j]->getFd());
                    delete _clients[j];
                    _clients.erase(_clients.begin() + j);
					continue;
                }
            }
			++j;
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

void Config::_delFd(int fd)
{
	if (fd < 0)
		return;
	
	// Vérifier si le descripteur est toujours valide
	struct stat statbuf;
	if (fstat(fd, &statbuf) < 0) {
		Logger::log("File descriptor " + Utils::toString(fd) + " already closed", INFO);
		return;
	}

	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) < 0) {
		Logger::log("Failed to remove fd " + Utils::toString(fd) + " from epoll instance: " + strerror(errno), ERROR);
	}
}

void Config::_initEpoll()
{
	_epollfd = epoll_create(1);
	if (_epollfd < 0)
		throw std::runtime_error("Failed to create epoll instance");
	
	for (size_t i = 0; i < _servers.size(); ++i)
	{
		_addFd(_servers[i]->getSockFd(), EPOLLIN | EPOLLET);
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
