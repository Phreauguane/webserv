#pragma once

#include "webserv.h"

enum methods
{
	GET,
	POST,
	DELETE,
	UNKNOWN
};

class Request
{
public:
	Request();
	Request(const Request&);
	Request(const std::string&);
	void parseRequest();
	void parseBody();
	void logData();
	Request& operator=(const Request&);
	~Request();
public:
	std::string request;
	std::string method;
	std::string path;
	std::string http_version;
	std::string host;
	std::string user_agent;
	std::vector<std::string> accept;
	std::map<std::string, std::string> attributes;
	bool keep_alive;
	std::string body;
	std::string files;
};
