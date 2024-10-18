#pragma once

#include "webserv.h"
#include "Request.h"
#include "Response.h"
#include "Location.h"

class CGI
{
public:
	CGI();
	CGI(const CGI&);
	void setup(char**);
	bool executeCGI(Request&, const std::string&, Location*, Response&);
	CGI& operator=(const CGI&);
	~CGI();
private:
	void _restoreStdOut();
	std::string _getExec(std::string);
	Response _execPHP(const std::string&, Request&);
	std::string _getQuery(const Request&);
	void _executeCommand(const std::string&, const std::string&, char**, char**, int*);
private:
	char **_env;
	int _stdout;
};
