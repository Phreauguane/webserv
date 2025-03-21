#pragma once

#include "webserv.h"
#include "Server.h"
#include "Response.h"
#include "Request.h"

class Server;

class Client
{
public:
	Client();
	Client(const Client&);
	Client(Server*);
	Client& operator=(const Client&);
	int getFd();
	std::string getLatestRequest();
	bool readRequest();
	bool sendResponse();
	void addResponse(Response&);
	void runRequests();
	~Client();

	class BufferOverflowException: public std::exception {
		virtual const char *what() const throw();
	};
private:
	Server* _server;
	int _fd;
	size_t _size;
	std::vector<Response> _reps;
	//Temp
	std::string _id;
};
