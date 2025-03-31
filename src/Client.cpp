#include "Client.h"
#include "Utils.h"

const char *Client::BufferOverflowException::what() const throw() {
	return ("Request buffer overflow");
}

Client::Client() : _server(NULL), _fd(-1), _size(0) {}

Client::Client(const Client& copy)
{
	*this = copy;
}

Client::Client(Server *server) : _server(server), _fd(-1), _size(0)
{
	_fd = accept(_server->getSockFd(), NULL, NULL);
	if (_fd < 0) {
		throw std::runtime_error("");  // Exception vide pour signaler "pas plus de connexions"
	}
	_id = "[" + Utils::toString(_fd) + "]";
	_size = _server->getMaxBodySize();
	Logger::log(_id + " Client connected to " + _server->getIp(), SUCCESS);
}

Client::~Client()
{
	Logger::log(_id + " Client disconnected", INFO);
	if (_fd >= 0)
		close(_fd);
}

Client& Client::operator=(const Client& copy)
{
	if (this == &copy)
		return *this;
	_server = copy._server;
	
	_fd = copy._fd;
	_size = copy._size;
	return *this;
}

int Client::getFd() {
	return _fd;
}

bool Client::readRequest() {
    char buffer[BUFFER_SIZE + 1];
    std::string requestData;
    bool requestComplete = false;
    
    while (!requestComplete) {
        std::memset(buffer, 0, BUFFER_SIZE + 1);
        ssize_t bytes = recv(_fd, buffer, BUFFER_SIZE, 0);
        
        if (bytes < 0)
            return false;
        
        if (bytes == 0) {
            if (requestData.empty())
                return false;
            requestComplete = true;
        } else {
            buffer[bytes] = '\0';
            requestData += buffer;
            
            if (requestData.find("\r\n\r\n") != std::string::npos)
                requestComplete = true;
        }
    }
	
	try {
		_server->pushRequest(new Request(std::string(buffer), this));
		return true;
	}
	catch (const std::exception& e) {
		Logger::log(e.what(), ERROR);
		return false;
	}
}

// bool Client::executeRequest()
// {
// 	if (_reqs.size() == 0)
// 		return false;

// 	int i = 0;
// 	while (_reqs[i] == NULL)++i;

// 	Request *request = _reqs[i];
// 	_reqs.erase(_reqs.begin(), _reqs.begin() + i + 1);

// 	try {
// 		_reps.push_back(_server->executeRequest(*request));
// 	}
// 	catch (...) { return false; }
// 	sendResponse();
	
// 	return true;
// }

bool Client::sendResponse()
{
	if (_reps.size() == 0)
	{
		Logger::log(_id + " Response buffer empty", ERROR);
		return true;
	}
	Response rep = _reps[0];
	std::string content = rep.build();
	Logger::log(_id + " Sending response", DEBUG);
	//Logger::log(rep.build(), TEXT);
	ssize_t len = content.size();
	ssize_t sent = send(_fd, content.c_str(), len, MSG_NOSIGNAL);
	if (sent == len)
		_reps.erase(_reps.begin());
	Logger::log(_id + " sent response", DEBUG);

	if (rep.attributes.find("Connection") != rep.attributes.end() && 
		rep.attributes["Connection"] == "close") {
		close(_fd);
	}

	return sent == len;
}

void Client::addResponse(Response &rep)
{
	_reps.push_back(rep);
}

void Client::runRequests()
{
	_server->runRequestsCli(this);
}