#pragma once

#include "webserv.h"
#include "Request.h"
#include "Response.h"
#include "Location.h"
#include "Session.h"
#include "ISessionManager.h"

class CGI
{
public:
	CGI();
	CGI(ISessionManager& server);
	CGI(const CGI&);
	void setup(char**);
	bool executeCGI(Request&, const std::string&, Location*, Response&);
	CGI& operator=(const CGI&);
	~CGI();
	Response _parseOutputPHP(std::string&, Request&);

private:
	void _restoreStdOut();
	std::string _getExec(std::string);
	Response _execPHP(const std::string&, Request&);
	std::string _getQuery(const Request&);
	void _executeCommand(const std::string&, const std::string&, char**, char**, int*);
	void _handlePHPSession(Request& req, Response& rep);

private:
	char **_env;
	int _stdout;
	ISessionManager* _server;
};
 