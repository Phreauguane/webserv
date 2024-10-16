#pragma once

#include "webserv.h"

class Response
{
public:
	Response();
	Response(const Response&);
	Response &operator=(const Response&);
	std::string build();
	std::string build_header();
	~Response();
public:
	bool ready;
	std::string http;
	int status;
	std::string phrase;
	std::map<std::string, std::string> attributes;
	std::string body;
};
