#pragma once

#include "webserv.h"
#include "Server.h"
#include "Response.h"
#include "Request.h"

class Server;

enum ReqStatus {
	NONE,
	DISCONNECT,
	INCOMPLETE,
	REQUEST_ERROR,
	FINISHED
};

enum ChunkState {
	CHUNK_SIZE,
	CHUNK_DATA,
	CHUNK_END,
	CHUNK_COMPLETE
};

struct BodyInfo {
    enum Type {
        BODY_NONE,
        BODY_CONTENT_LENGTH,
        BODY_CHUNKED,
        BODY_UNKNOWN
    };
    
    Type type;
    size_t length;

    BodyInfo() : type(BODY_NONE), length(0) {}
    BodyInfo(Type t, size_t l) : type(t), length(l) {}
};

class Client
{
public:
	Client();
	Client(const Client&);
	Client(Server*);
	Client& operator=(const Client&);
	int getFd();
	std::string getLatestRequest();
	ReqStatus readRequest();
	bool isIncomplete() const;
	void sendContinue();
	bool sendResponse();
	bool validate();
	bool checkTimeout(size_t) const;
	void addResponse(Response&);
	void runRequests();
	void resetState();
	void setServer(Server*);
	~Client();
private:
	ReqStatus _readLengthBody();
	ReqStatus _readChunkedBody();

	ReqStatus _handleReadErr();
	void _pushRequest();
	void _resetState();
	void _saveReadState(size_t, size_t, ReqStatus);
	bool _isHeaderComplete();
	BodyInfo _identifyBodyType(const std::string&);
private:
	Server* _server;
	int _fd;
	size_t _size;
	size_t _lastAction;
	std::vector<Response> _reps;

	// request reading
	std::vector<char> _requestData;
	size_t _header;
	size_t _readPos;
	size_t _remaining;
	ReqStatus _status;

	// header data
	bool _headerComplete;
	BodyInfo _info;
	ChunkState _chunkState;
	std::vector<char> _chunk;
};
