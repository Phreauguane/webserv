#include "Client.h"
#include "Utils.h"

Client::Client() : _server(NULL), _fd(-1), _size(0) {}

Client::Client(const Client& copy)
{
	*this = copy;
}

Client::Client(Server *server) : _server(server), _fd(-1), _size(0), _headerComplete(false), _chunkState(CHUNK_SIZE)
{
    this->_resetState();
	_fd = accept(_server->getSockFd(), NULL, NULL);
	if (_fd < 0)
		throw std::runtime_error("connection failed");
    _lastAction = time(NULL);
	_size = _server->getMaxBodySize();
	Logger::log("Client connected to " + _server->getIp(), SUCCESS);
}

Client::~Client()
{
	Logger::log("Client disconnected", INFO);
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

bool Client::checkTimeout(size_t now) const {
    return ((now - _lastAction) * 1000) > _server->getTimeout();
}

bool Client::_isHeaderComplete() {
    return Utils::findIndex(_requestData, "\r\n\r\n") != -1;
}

BodyInfo Client::_identifyBodyType(const std::string& rawHeaders) {
    std::string::size_type firstLineEnd = rawHeaders.find("\r\n");
    if (firstLineEnd == std::string::npos) {
        return BodyInfo(BodyInfo::BODY_UNKNOWN, 0);
    }
    
    std::string firstLine = rawHeaders.substr(0, firstLineEnd);
    std::string::size_type methodEnd = firstLine.find(' ');
    if (methodEnd == std::string::npos) {
        return BodyInfo(BodyInfo::BODY_UNKNOWN, 0);
    }
    
    std::string method = firstLine.substr(0, methodEnd);
    
    bool hasContentLength = false;
    bool hasTransferEncoding = false;
    bool isChunked = false;
    size_t contentLength = 0;
    
    std::string::size_type pos = firstLineEnd + 2;
    while (pos < rawHeaders.size()) {
        std::string::size_type lineEnd = rawHeaders.find("\r\n", pos);
        if (lineEnd == std::string::npos) {
            break;
        }
        
        std::string line = rawHeaders.substr(pos, lineEnd - pos);
        pos = lineEnd + 2;
        
        if (line.empty()) {
            continue;
        }
        
        std::string::size_type colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            continue;
        }
        
        std::string headerName = line.substr(0, colonPos);
        std::string headerValue = line.substr(colonPos + 1);
        
        headerName = Utils::trimString(headerName);
        headerValue = Utils::trimString(headerValue);
        
        std::string lowerHeaderName = Utils::toLowerCase(headerName);
        
        if (lowerHeaderName == "content-length") {
            hasContentLength = true;
            
            bool isValidNumber = true;
            for (size_t i = 0; i < headerValue.size(); ++i) {
                if (!isdigit(headerValue[i])) {
                    isValidNumber = false;
                    break;
                }
            }
            
            if (isValidNumber) {
                contentLength = static_cast<size_t>(std::atol(headerValue.c_str()));
            } else {
                hasContentLength = false;
            }
        } else if (lowerHeaderName == "transfer-encoding") {
            hasTransferEncoding = true;
            
            std::string lowerValue = Utils::toLowerCase(headerValue);
            isChunked = (lowerValue.find("chunked") != std::string::npos);
        }
    }
    
    if (method == "GET" || method == "DELETE") {
        if (!hasContentLength && !hasTransferEncoding) {
            return BodyInfo(BodyInfo::BODY_NONE, 0);
        }
    }
    
    if (hasTransferEncoding && isChunked) {
        return BodyInfo(BodyInfo::BODY_CHUNKED, 0);
    }
    
    if (hasContentLength) {
        return BodyInfo(BodyInfo::BODY_CONTENT_LENGTH, contentLength);
    }
    
    if (method == "POST") {
        return BodyInfo(BodyInfo::BODY_UNKNOWN, 0);
    }
    
    return BodyInfo(BodyInfo::BODY_NONE, 0);
}

void Client::_resetState() {
    _requestData = std::vector<char>(0);
    _chunk = std::vector<char>(0);
    _readPos = 0;
    _remaining = 0;
    _status = NONE;
    _headerComplete = false;
    _info = BodyInfo();
    _chunkState = CHUNK_SIZE;
}

void Client::_saveReadState(size_t pos, size_t remaining, ReqStatus status) {
    _readPos = pos;
    _remaining = remaining;
    _status = status;
}

ReqStatus Client::_handleReadErr() {
    if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
        Logger::log("Read signal: " + std::string(errno == EAGAIN ? "EAGAIN" : errno == EWOULDBLOCK ? "EWOULDBLOCK" : "EINTR"), DEBUG);
        _saveReadState(_readPos, _remaining, INCOMPLETE);
        return INCOMPLETE;
    } else {
        Logger::log("Read error: " + std::string(strerror(errno)), ERROR);
        _saveReadState(_readPos, _remaining, DISCONNECT);
        return DISCONNECT;
    }
}

ReqStatus Client::_readLengthBody() {
    if (_info.length == 0) {
        Logger::log("length = 0 -> FINISHED", DEBUG);
        _pushRequest();
        _resetState();
        return FINISHED;
    }
    
    _remaining = (_header + _info.length) - _requestData.size();
    Logger::log("[Remaining: " + Utils::toString(_remaining) + "]", DEBUG);

    std::vector<char> buffer(_remaining + 1);
    ssize_t bytes_ = recv(_fd, buffer.data(), _remaining, MSG_DONTWAIT);

    if (bytes_ < 0) {
        return _handleReadErr();
    } else if (bytes_ < static_cast<ssize_t>(_remaining)) {
        _requestData.insert(_requestData.end(), buffer.begin(), buffer.begin() + bytes_);
        _saveReadState(_readPos + bytes_, _remaining - bytes_, INCOMPLETE);
        return INCOMPLETE;
    } else {
        Logger::log("Remaining before: " + Utils::toString(_remaining), DEBUG);
        Logger::log("bytes: " + Utils::toString(bytes_), DEBUG);
        _requestData.insert(_requestData.end(), buffer.begin(), buffer.begin() + bytes_);
        _remaining = (_header + _info.length) - _requestData.size();
        Logger::log("Remaining: " + Utils::toString(_remaining), DEBUG);
        if (_remaining == 0) {
            Logger::log("Read whole body -> FINISHED", DEBUG);
            _pushRequest();
            _resetState();
            return FINISHED;
        }
        _saveReadState(_readPos + bytes_, _remaining - bytes_, INCOMPLETE);
        return INCOMPLETE;
    }
}

bool Client::isIncomplete() const {
    return _status == INCOMPLETE;
}

ReqStatus Client::_readChunkedBody() {
    while (true) {
        if (_chunkState == CHUNK_SIZE) {
            char buff = '\0';
            ssize_t bytes = recv(_fd, &buff, 1, MSG_DONTWAIT);

            if (bytes < 0) {
                return _handleReadErr();
            } else if (bytes == 0) {
                Logger::log("Reached EOF", DEBUG);
                _saveReadState(_readPos, _remaining, INCOMPLETE);
                return INCOMPLETE;
            } else {
                _chunk.push_back(buff);
                if (Utils::find(_chunk, "\r\n") != _chunk.end()) {
                    if (_chunk[0] == '0' && _chunk[1] == '\r' && _chunk[2] == '\n') {
                        _chunkState = CHUNK_COMPLETE;
                        continue;
                    }
                    std::string sizeStr(_chunk.begin(), _chunk.begin() + _chunk.size() - 2);
                    _info.length = static_cast<size_t>(std::strtoul(sizeStr.c_str(), NULL, 16));
                    _chunk = std::vector<char>();
                    _chunkState = CHUNK_DATA;
                }
            }
        }

        if (_chunkState == CHUNK_DATA) {
            Logger::log("[Chunk-Size: " + Utils::toString(_info.length) + "]", DEBUG);
            _remaining = _info.length - _chunk.size();

            std::vector<char> buff(_remaining + 1);
            ssize_t bytes = recv(_fd, buff.data(), _remaining, MSG_DONTWAIT);

            if (bytes < 0) {
                return _handleReadErr();
            } else if (bytes < static_cast<ssize_t>(_remaining)) {
                _chunk.insert(_chunk.end(), buff.begin(), buff.begin() + bytes);
                Logger::log("Reached EOF", DEBUG);
                _saveReadState(_readPos, _remaining, INCOMPLETE);
                return INCOMPLETE;
            } else {
                _chunk.insert(_chunk.end(), buff.begin(), buff.begin() + bytes);
                _requestData.insert(_requestData.end(), _chunk.begin(), _chunk.end());
                Logger::log("Read chunk", DEBUG);
                _chunk = std::vector<char>();
                _chunkState = CHUNK_END;
            }
        }

        if (_chunkState == CHUNK_END) {
            char buff = '\0';
            ssize_t bytes = recv(_fd, &buff, 1, MSG_DONTWAIT);

            if (bytes < 0) {
                return _handleReadErr();
            } else if (bytes == 0) {
                Logger::log("Reached EOF", DEBUG);
                _saveReadState(_readPos, _remaining, INCOMPLETE);
                return INCOMPLETE;
            } else {
                _chunk.push_back(buff);
                if (Utils::find(_chunk, "\r\n") != _chunk.end()) {
                    _chunk = std::vector<char>();
                    _chunkState = CHUNK_SIZE;
                }
            }
        }

        if (_chunkState == CHUNK_COMPLETE) {
            Logger::log("CHUNK COMPLETE -> FINISHED", DEBUG);
            _pushRequest();
            _resetState();
            return FINISHED;
        }
    }
}

void Client::resetState() {
    _resetState();
}

ReqStatus Client::readRequest() {
    if (!Utils::isSocketValid(_fd)) {
        Logger::log("Invalid socket, disconnecting...", ERROR);
        _saveReadState(_readPos, _remaining, DISCONNECT);
        return DISCONNECT;
    }

    Logger::log("Received request. headerComplete: " + std::string(_headerComplete ? "true" : "false"), DEBUG);

    while (true) {
        if (!_headerComplete && !_isHeaderComplete()) {
            char buffer = '\0';
            ssize_t bytes = recv(_fd, &buffer, 1, MSG_DONTWAIT);
            if (bytes > 0) {
                _requestData.push_back(buffer);
                _lastAction = time(NULL); // reset timeout
            }

            if (bytes < 0) {
                return _handleReadErr();
            } else if (bytes == 0) {
                Logger::log("Reached EOF", DEBUG);
                _saveReadState(_readPos, _remaining, INCOMPLETE);
                return INCOMPLETE;
            }
        }

        if (_headerComplete || _isHeaderComplete()) {
            Logger::log("headerComplete: " + std::string(_headerComplete ? "true" : "false"), DEBUG);
            Logger::log("Request size: " + Utils::toString(_requestData.size()), DEBUG);
            if (!_headerComplete) {
                Logger::log("Header read", DEBUG);
                Logger::log(std::string(_requestData.begin(), _requestData.end()), TEXT);
                _headerComplete = true;
                _header = Utils::findIndex(_requestData, "\r\n\r\n") + 4;
                _info = _identifyBodyType(std::string(_requestData.begin(), _requestData.end()));
                Logger::log("header index: " + Utils::toString(_header), DEBUG);
                if (_info.type == BodyInfo::BODY_CONTENT_LENGTH && _info.length > _server->getMaxBodySize()) {
                    Logger::log("Content-Length too large: " + Utils::toString( _info.length) + 
                        " > " + Utils::toString(_server->getMaxBodySize()), ERROR);
                    _requestData.clear();
                    _resetState();
                    Response rep = _server->errorPage(413);
                    _reps.push_back(rep);
                    return FINISHED;
                }
            }
            if (_info.type == BodyInfo::BODY_CONTENT_LENGTH) {
                return _readLengthBody();
            } else if (_info.type == BodyInfo::BODY_CHUNKED) {
                return _readChunkedBody();
            } else { // BODY_NONE or BODY_UNKNOWN -> handled later
                Logger::log("BODY NONE/UNKOWN -> FINISHED", DEBUG);
                _pushRequest();
                _resetState();
                return FINISHED;
            }
        } else {
            continue;
        }
    }

    Logger::log("Unhandled situation. Disconnecting...", ERROR);
    _saveReadState(_readPos, _remaining, DISCONNECT);
    return DISCONNECT;
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
	ssize_t len = content.size();
	ssize_t sent = send(_fd, content.c_str(), len, MSG_NOSIGNAL);
	if (sent == len)
		_reps.erase(_reps.begin());
	Logger::log("sent response", DEBUG);

	if (rep.attributes.find("Connection") != rep.attributes.end() && 
		rep.attributes["Connection"] == "close") {
		close(_fd);
	}

	return sent == len;
}

void Client::addResponse(Response &rep) {
	_reps.push_back(rep);
}

void Client::runRequests() {
	_server->runRequestsCli(this);
}

void Client::_pushRequest() {
    // Si les données de la requête sont vides, on ignore simplement
    if (_requestData.empty()) {
        return;
    }

    Request *req = NULL;
    try {
        req = new Request(_requestData, this);
        _server->pushRequest(req);
    } catch (const std::exception &e) {
        Logger::log("Failed to create request" + std::string(e.what()), DEBUG);
        if (req)
            delete req;
    }
}