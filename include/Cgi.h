#pragma once

#include "webserv.h"
#include "Request.h"
#include "Response.h"
#include "Location.h"
#include "Session.h"
#include "ISessionManager.h"

#define MAX_POST_SIZE 10485760  // 10MB par exemple

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
	Response _execPython(const std::string&, Request&);
	Response _execC(const std::string&, Request&);
	std::string _compileCProgram(const std::string&);

private:
	char **_env;
	int _stdout;
	ISessionManager* _server;
};
 