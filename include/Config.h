#pragma once

#include "Server.h"
#include "Client.h"

class Config
{
public:
	Config();
	Config(char**);
	Config(const Config&);
	bool hasClone(const std::string&);
	void load(const std::string&);
	void setup();
	void run(bool*);
	Config& operator=(const Config&);
	~Config();

	class InvalidConfig: public std::exception {
		const char *what() const throw();
	};
private:
	void _createServers();
	void _setupServers();
	void _initEpoll();
	void _addFd(int, uint32_t);
	void _modFd(int, uint32_t);
	void _delFd(int);
	void _disconnectClient(size_t);
	void _checkForConnections(size_t);
	void _handleRequests(size_t);
	void _checkForTimeouts();
	void _runRequests();
private:
	char **_env;
	int _epollfd;
	struct epoll_event _epollevents[MAX_EVENTS];
	std::string _source, _filename;
	std::vector<Server*> _servers;
	std::vector<Client*> _clients;
};
