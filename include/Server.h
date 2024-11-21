#pragma once

#include "webserv.h"
#include "Location.h"
#include "Request.h"
#include "Response.h"
#include "Cgi.h"
#include "Session.h"
#include "ISessionManager.h"

enum req_type
{
	T_FILE,
	T_DIR,
	T_NFD
};

class Server : public ISessionManager
{
public:
	Server();
	Server(const Server&);
	Server(const std::string&, char**);
	Server& operator=(const Server&);
	in_addr_t getHost() const;
	void setup();
	int getSockFd() const;
	std::string getIp();
	Response executeRequest(Request&);
	size_t getMaxBodySize();
	void printDetails() const;
	~Server();
	virtual bool hasSession(const std::string& sessionId);
	virtual Session* getSession(const std::string& sessionId);
private:
	Response _get(const Request&);
	Response _post(const Request&);
	Response _delete(const Request&);
	Response _readFile(const Request&, const std::string&);
	Response _autoIndex(const std::string&);
	Response _errorPage(unsigned int);
	req_type _getType(const std::string&);
	std::string _listDirectory(const std::string&);
	std::string _findResourcePath(const Request&, Location*);
	void _parseSource(const std::string&);
	void _setupServAddr();
	void _loadTypes();
	Location *_getLocation(const std::string&);
	void _handleSession(Request& req, Response& rep);
	std::string _generateSessionId();
	void _cleanExpiredSessions();
private:
	char **_env;
	CGI cgiHandler;
	std::string _name, _ip_addr;
	bool _ready;
	struct sockaddr_in _servaddr;
	in_addr_t _host;
	int _port, _sockfd;
	Location *_root_loc;
	std::map<unsigned int, std::string> _error_pages;
	std::map<std::string, std::string> _types;
	unsigned int _max_body_size;
	std::map<std::string, Session*> _sessions;
};
