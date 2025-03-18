#pragma once

#include "webserv.h"
#include "Server.h"
#include "Response.h"
#include "Request.h"

class Client
{
public:
	Client();
	Client(const Client&);
	Client(Server*);
	Client& operator=(const Client&);
	int getFd();
	std::string getRequest();
	bool sendResponse();
	bool readRequest();
	~Client();
private:
	Server* _server;
	Request* _request;
	int _fd;
	size_t _size;
	std::vector<Response> _reps;
	//Temp
	std::string _id;
};
