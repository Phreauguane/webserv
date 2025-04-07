#include "Request.h"
#include "Utils.h"

Request::Request()
{
	//
};

Request::Request(const Request& copy)
{
	*this = copy;
}

Request::Request(const std::vector<char>& req, Client *client)
{
	this->request = req;
	this->client = client;
	try	{
		parseRequest();
	}
	catch(const std::exception& e) {
		throw;
	}
}

void Request::parseRequest()
{
	Logger::log("Received request:", DEBUG);
	Logger::log(request, DEBUG);
	size_t headerEnd = Utils::findIndex(request, "\r\n\r\n");
	if (headerEnd == std::string::npos)
	{
		Logger::log("Invalid request format: no header-body separator found", ERROR);
		return;
	}

	if (validateRequest() != VALID) {
        throw std::runtime_error("Invalid request");
    }

	std::string headers = std::string(request.begin(), request.begin() + headerEnd);
	
	std::vector<std::string> lines = Utils::splitString(headers, "\n");
	for (size_t i = 0; i < lines.size(); i++)
	{
		if (i == 0) {
			std::vector<std::string> strs = Utils::splitString(lines[i], " ");
			if (strs.size() != 3)
				throw std::runtime_error("Invalid Header : " + lines[i]);
			method = strs[0];
			path = strs[1];
			http_version = strs[2];
		}
		else {
			std::vector<std::string> strs = Utils::splitString(lines[i], ": ");
			if (strs.size() >= 2)
			{
				std::string key = strs[0];
				std::string value = strs[1];
				
				for (size_t j = 2; j < strs.size(); j++)
					value += ": " + strs[j];
					
				if (key == "host")
					host = value;
				else if (key == "Accept")
					accept = Utils::splitString(value, ",");
				else if (key == "Connection")
					keep_alive = (value == "keep-alive");
				else
					attributes[key] = value;
			}
		}
	}

	if (headerEnd + std::string("\r\n\r\n").size() < request.size())
	{
		body = std::vector<char>(request.begin() + headerEnd + 4, request.end());
		Logger::log("Body size after initial parsing: " + Utils::toString(body.size()), DEBUG);
		
		std::map<std::string, std::string>::iterator it = attributes.find("Content-Type");
		if (it != attributes.end() && it->second.find("multipart/form-data") != std::string::npos) {
			parseMultipartFormData(/* miaou */);
		}
	}
}

void Request::parseMultipartFormData()
{
	std::string contentType = attributes["Content-Type"];
	size_t boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos)
	{
		Logger::log("No boundary found in Content-Type", ERROR);
		return;
	}
	
	std::string boundary = contentType.substr(boundaryPos + 9);
	std::string boundaryStart = "--" + boundary;
	std::string boundaryEnd = "--" + boundary + "--";
	
	ssize_t startPos = Utils::findIndex(body, boundaryStart);
	if (startPos == -1)
	{
		Logger::log("Start boundary not found", ERROR);
		return;
	}
	
	startPos = Utils::findIndex(body, "\r\n", startPos);
	if (startPos == -1)
	{
		Logger::log("Invalid boundary format", ERROR);
		return;
	}
	startPos += 2;
	
	ssize_t headerEnd = Utils::findIndex(body, "\r\n\r\n", startPos);
	if (headerEnd == -1)
	{
		Logger::log("Header end not found", ERROR);
		return;
	}
	
	ssize_t contentStart = headerEnd + 4;
	
	ssize_t contentEnd = Utils::findIndex(body, "\r\n" + boundaryEnd, contentStart);
	if (contentEnd == -1)
	{
		contentEnd = Utils::findIndex(body, "\r\n" + boundaryStart, contentStart);
		if (contentEnd == -1)
		{
			Logger::log("End boundary not found", ERROR);
			return;
		}
	}
	
	if (contentEnd > contentStart)
	{
		body = std::vector<char>(body.begin() + contentStart, body.begin() + contentEnd);
	}
	else
	{
		Logger::log("Invalid content boundaries", ERROR);
		return;
	}
}

Request& Request::operator=(const Request& copy)
{
	if (this == &copy)
		return *this;
	request = copy.request;
	method = copy.method;
	path = copy.path;
	http_version = copy.http_version;
	host = copy.host;
	accept = copy.accept;
	keep_alive = copy.keep_alive;
	body = copy.body;
	return *this;
}

void Request::logData()
{
	Logger::log("method = " + method, TEXT);
	Logger::log("path = " + path, TEXT);
	Logger::log("http_version = " + http_version, TEXT);
	std::cout << std::endl;
	Logger::log("host = " + host, TEXT);
	Logger::log("user_agent = " + user_agent, TEXT);
	if (keep_alive)
		Logger::log("keep_alive = TRUE", TEXT);
	else
		Logger::log("keep_alive = FALSE", TEXT);
	Logger::log(body, TEXT);
}

Request::~Request()
{
	attributes.clear();
	accept.clear();
}

ValidationError Request::validateFirstLine(const std::vector<char> &req) {
	std::string reqStr = req.data();
    std::vector<std::string> lines = Utils::splitString(reqStr, "\n");
	
    std::vector<std::string> firstLine = Utils::splitString(lines[0], " ");
    if (firstLine.size() != 3) {
        Logger::log("Invalid first line format", ERROR);
        return INVALID_BODY_FORMAT;
    }

    std::string method = firstLine[0];
    if (method != "GET" && method != "POST" && method != "DELETE") {
        Logger::log("Invalid HTTP method: " + method, ERROR);
        return INVALID_METHOD;
    }

    std::string http_version = firstLine[2];
    if (http_version != "HTTP/1.1" && http_version != "HTTP/1.0") {
        Logger::log("Invalid HTTP version: " + http_version, ERROR);
        return INVALID_VERSION;
    }

    std::string path = firstLine[1];
    if (path.empty() || path[0] != '/') {
        Logger::log("Invalid URL: " + path, ERROR);
        return INVALID_URL;
    }

	return VALID;
}

ValidationError Request::validateRequest() {
    size_t headerEnd = Utils::findIndex(request, "\r\n\r\n");
    if (headerEnd == std::string::npos) {
        Logger::log("Invalid request format: no header-body separator found", ERROR);
        return INVALID_BODY_FORMAT;
    }

    std::string headers = std::string(request.begin(), request.begin() + headerEnd);
    std::vector<std::string> lines = Utils::splitString(headers, "\n");
    
    if (lines.empty()) {
        Logger::log("Empty request", ERROR);
        return INVALID_BODY_FORMAT;
    }

    std::vector<std::string> firstLine = Utils::splitString(lines[0], " ");
    if (firstLine.size() != 3) {
        Logger::log("Invalid first line format", ERROR);
        return INVALID_BODY_FORMAT;
    }

    std::string method = firstLine[0];
    if (method != "GET" && method != "POST" && method != "DELETE") {
        Logger::log("Invalid HTTP method: " + method, ERROR);
        return INVALID_METHOD;
    }

    std::string http_version = firstLine[2];
    if (http_version != "HTTP/1.1" && http_version != "HTTP/1.0") {
        Logger::log("Invalid HTTP version: " + http_version, ERROR);
        return INVALID_VERSION;
    }

    std::string path = firstLine[1];
    if (path.empty() || path[0] != '/') {
        Logger::log("Invalid URL: " + path, ERROR);
        return INVALID_URL;
    }

    bool hostFound = false;
    for (size_t i = 1; i < lines.size(); i++) {
        std::vector<std::string> header = Utils::splitString(lines[i], ": ");
        if (header.size() >= 2 && Utils::toLowerCase(header[0]) == "host") {
            hostFound = true;
            break;
        }
    }
    if (!hostFound) {
        Logger::log("Missing Host header", ERROR);
        return MISSING_HOST;
    }

    if (method == "POST") {
        bool hasContentType = false;
        bool hasContentLength = false;
        bool validContentLength = true;

        for (size_t i = 1; i < lines.size(); i++) {
            std::vector<std::string> header = Utils::splitString(lines[i], ": ");
            if (header.size() >= 2) {
                std::string key = Utils::toLowerCase(header[0]);
                std::string value = header[1];

                if (key == "content-type") {
                    hasContentType = true;
                } else if (key == "content-length") {
                    hasContentLength = true;
                    for (size_t j = 0; j < value.size(); j++) {
                        if (!isdigit(value[j])) {
                            validContentLength = false;
                            break;
                        }
                    }
                }
            }
        }

        if (!hasContentType) {
            Logger::log("Missing Content-Type for POST request", ERROR);
            return INVALID_CONTENT_TYPE;
        }

        if (hasContentLength && !validContentLength) {
            Logger::log("Invalid Content-Length format", ERROR);
            return INVALID_CONTENT_LENGTH;
        }
    }

    return VALID;
}