#pragma once

#include "webserv.h"
#include "Client.h"

class Client;

enum methods {
	GET,
	POST,
	DELETE,
	UNKNOWN
};

enum ValidationError {
    VALID,
    INVALID_METHOD,
    INVALID_VERSION,
    INVALID_URL,
    MISSING_HOST,
    INVALID_CONTENT_LENGTH,
    INVALID_CONTENT_TYPE,
    INVALID_BODY_FORMAT
};

class Request
{
public:
	Request();
	Request(const Request&);
	Request(const std::vector<char>&, Client*);
	ValidationError validateRequest();
	static ValidationError validateFirstLine(const std::vector<char>&);
	void parseRequest();
	void parseMultipartFormData();
	void logData();
	Request& operator=(const Request&);
	~Request();
public:
	std::vector<char> request;
	std::string method;
	std::string path;
	std::string http_version;
	std::string host;
	std::string user_agent;
	std::vector<std::string> accept;
	std::map<std::string, std::string> attributes;
	bool keep_alive;
	std::vector<char> body;
	std::string files;
	//
	Client *client;
};
