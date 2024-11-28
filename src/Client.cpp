#include "Client.h"

Client::Client() : _server(NULL), _request(NULL), _fd(-1), _size(0) {}

Client::Client(const Client& copy) : _request(NULL)
{
	*this = copy;
}

Client::Client(Server *server) : _server(server), _request(NULL), _fd(-1), _size(0)
{
	_fd = accept(_server->getSockFd(), NULL, NULL);
	if (_fd < 0)
		throw std::runtime_error("Failed to accept connection to " + _server->getIp());
	_size = _server->getMaxBodySize();
	Logger::log("Client connected to " + _server->getIp(), SUCCESS);
}

Client::~Client()
{
	if (_request)
	{
		delete _request;
		_request = NULL;
	}
	if (_fd >= 0)
		close(_fd);
}

Client& Client::operator=(const Client& copy)
{
	if (this == &copy)
		return *this;
	_server = copy._server;
	if (_request)
		delete _request;
	_request = copy._request ? new Request(*copy._request) : NULL;
	_fd = copy._fd;
	_size = copy._size;
	_reps = copy._reps;
	return *this;
}

int Client::getFd()
{
	return _fd;
}

bool Client::readRequest()
{
	char buffer[BUFFER_SIZE + 1];
	ssize_t bytes = recv(_fd, buffer, BUFFER_SIZE, 0);
	
	if (bytes <= 0)
		return false;
	
	buffer[bytes] = '\0';
	
	try {
		Request* newRequest = new Request(std::string(buffer));
		Request* oldRequest = _request;
		_request = newRequest;
		
		try {
			_reps.push_back(_server->executeRequest(*_request));
			if (oldRequest)
				delete oldRequest;
		}
		catch (...) {
			_request = oldRequest;
			delete newRequest;
			throw;
		}
	}
	catch (const std::exception& e) {
		Logger::log(e.what(), ERROR);
		return false;
	}
	
	return true;
}

bool Client::sendResponse()
{
	if (_reps.size() == 0)
	{
		Logger::log("Response buffer empty", ERROR);
		return true;
	}
	Response rep = _reps[0];
	std::string content = rep.build();
	Logger::log("Sending response", DEBUG);
	//Logger::log(rep.build(), TEXT);
	ssize_t len = content.size();
	ssize_t sent = send(_fd, content.c_str(), len, MSG_NOSIGNAL);
	if (sent == len)
		_reps.erase(_reps.begin());
	Logger::log("sent response", DEBUG);
	return sent == len;
}

std::string Client::getRequest()
{
	return _request ? _request->request : "";
}
