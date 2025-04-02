#include "Server.h"
#include "Utils.h"

Server::Server()
{
	//
}

Server::Server(const Server& copy)
{
	*this = copy;
}

void Server::_parseSource(const std::string& source)
{
	size_t position = 0;
	std::vector<std::string> lines = Utils::splitString(source, "\n");
	
	for (size_t i = 0; i < lines.size(); ++i) {
		position = source.find(lines[i], position + 1);
		std::string line = Utils::removeSemicolon(Utils::removeWSpaces(lines[i]));
		std::vector<std::string> strs = Utils::splitString(line, " ");
		
		if (strs[0] == "server" && i == 0)
		{}
		else if (strs[0] == "location")
		{
			Utils::verify_args(strs, 2, 3);
			std::string src = Utils::readBrackets(source, position);
			std::vector<std::string> temp = Utils::splitString(src, "\n");
			if (strs[1] == "/")
				_root_loc->parseConfig(src);
			else
				_root_loc->addChildren(src);
			i += temp.size() - 1;
			continue;
		}
		else if (strs[0] == "server_name" || strs[0] == "name")
		{
			Utils::verify_args(strs, 2, 2);
			_name = strs[1];
		}
		else if (strs[0] == "host")
		{
			Utils::verify_args(strs, 2, 2);
			_host = inet_addr(strs[1].c_str());
			_ip_addr = strs[1];
		}
		else if (strs[0] == "listen")
		{
			Utils::verify_args(strs, 2, 2);
			_port = std::atoi(strs[1].c_str());
			if (_port > 9999)
				throw std::runtime_error("Invalid port format : " + strs[1]);
		}
		else if (strs[0] == "error_page")
		{
			Utils::verify_args(strs, 3, 3);
			unsigned int code = std::atoi(strs[1].c_str());
			_error_pages[code] = strs[2];
		}
		else if (strs[0] == "client_max_body_size")
		{
			Utils::verify_args(strs, 2, 2);
			_max_body_size = std::atoi(strs[1].c_str());
		}
		else if (strs[0] == "timeout_ms")
		{
			Utils::verify_args(strs, 2, 2);
			_timeout = static_cast<size_t>(strtoul(strs[1].c_str(), NULL, 10));
		}
		else if (strs[0] == "{" || strs[0] == "}")
		{}
		else
			_root_loc->parseLine(strs);
	}
}

Server::Server(const std::string& source, char **env): _timeout(DEFAULT_TIMEOUT)
{
	_env = env;
	cgiHandler = new CGI(this);
	cgiHandler->setup(_env);
	_root_loc = new Location(_env);
	_parseSource(source);
}

size_t Server::getTimeout() const {
	return _timeout;
}

void Server::pushRequest(Request *req)
{
	_reqs.push_back(req);
}

void Server::_loadTypes()
{
	_types[".html"] = "text/html";
	_types[".php"] = "application/x-httpd-php";
	_types[".htm"] = "text/html";
	_types[".shtml"] = "text/html";
	_types[".css"] = "text/css";
	_types[".xml"] = "text/xml";
	_types[".gif"] = "image/gif";
	_types[".jpeg"] = "image/jpeg";
	_types[".jpg"] = "image/jpeg";
	_types[".js"] = "application/javascript";
	_types[".atom"] = "application/atom+xml";
	_types[".rss"] = "application/rss+xml";
	_types[".mml"] = "text/mathml";
	_types[".txt"] = "text/plain";
	_types[".jad"] = "text/vnd.sun.j2me.app-descriptor";
	_types[".wml"] = "text/vnd.wap.wml";
	_types[".htc"] = "text/x-component";
	_types[".png"] = "image/png";
	_types[".svg"] = "image/svg+xml";
	_types[".svgz"] = "image/svg+xml";
	_types[".tif"] = "image/tiff";
	_types[".tiff"] = "image/tiff";
	_types[".wbmp"] = "image/vnd.wap.wbmp";
	_types[".webp"] = "image/webp";
	_types[".ico"] = "image/x-icon";
	_types[".jng"] = "image/x-jng";
	_types[".bmp"] = "image/x-ms-bmp";
	_types[".woff"] = "font/woff";
	_types[".woff2"] = "font/woff2";
	_types[".jar"] = "application/java-archive";
	_types[".war"] = "application/java-archive";
	_types[".ear"] = "application/java-archive";
	_types[".json"] = "application/json";
	_types[".hqx"] = "application/mac-binhex40";
	_types[".doc"] = "application/msword";
	_types[".pdf"] = "application/pdf";
	_types[".ps"] = "application/postscript";
	_types[".eps"] = "application/postscript";
	_types[".ai"] = "application/postscript";
	_types[".rtf"] = "application/rtf";
	_types[".m3u8"] = "application/vnd.apple.mpegurl";
	_types[".kml"] = "application/vnd.google-earth.kml+xml";
	_types[".kmz"] = "application/vnd.google-earth.kmz";
	_types[".xls"] = "application/vnd.ms-excel";
	_types[".eot"] = "application/vnd.ms-fontobject";
	_types[".ppt"] = "application/vnd.ms-powerpoint";
	_types[".odg"] = "application/vnd.oasis.opendocument.graphics";
	_types[".odp"] = "application/vnd.oasis.opendocument.presentation";
	_types[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_types[".odt"] = "application/vnd.oasis.opendocument.text";
	_types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_types[".wmlc"] = "application/vnd.wap.wmlc";
	_types[".7z"] = "application/x-7z-compressed";
	_types[".cco"] = "application/x-cocoa";
	_types[".jardiff"] = "application/x-java-archive-diff";
	_types[".jnlp"] = "application/x-java-jnlp-file";
	_types[".run"] = "application/x-makeself";
	_types[".pl"] = "application/x-perl";
	_types[".pm"] = "application/x-perl";
	_types[".prc"] = "application/x-pilot";
	_types[".pdb"] = "application/x-pilot";
	_types[".rar"] = "application/x-rar-compressed";
	_types[".rpm"] = "application/x-redhat-package-manager";
	_types[".sea"] = "application/x-sea";
	_types[".swf"] = "application/x-shockwave-flash";
	_types[".sit"] = "application/x-stuffit";
	_types[".tcl"] = "application/x-tcl";
	_types[".tk"] = "application/x-tcl";
	_types[".der"] = "application/x-x509-ca-cert";
	_types[".pem"] = "application/x-x509-ca-cert";
	_types[".crt"] = "application/x-x509-ca-cert";
	_types[".xpi"] = "application/x-xpinstall";
	_types[".xhtml"] = "application/xhtml+xml";
	_types[".xspf"] = "application/xspf+xml";
	_types[".zip"] = "application/zip";
	_types[".bin"] = "application/octet-stream";
	_types[".exe"] = "application/octet-stream";
	_types[".dll"] = "application/octet-stream";
	_types[".deb"] = "application/octet-stream";
	_types[".dmg"] = "application/octet-stream";
	_types[".iso"] = "application/octet-stream";
	_types[".img"] = "application/octet-stream";
	_types[".msi"] = "application/octet-stream";
	_types[".msp"] = "application/octet-stream";
	_types[".msm"] = "application/octet-stream";
	_types[".mid"] = "audio/midi";
	_types[".midi"] = "audio/midi";
	_types[".kar"] = "audio/midi";
	_types[".mp3"] = "audio/mpeg";
	_types[".ogg"] = "audio/ogg";
	_types[".m4a"] = "audio/x-m4a";
	_types[".ra"] = "audio/x-realaudio";
	_types[".3gpp"] = "video/3gpp";
	_types[".3gp"] = "video/3gpp";
	_types[".ts"] = "video/mp2t";
	_types[".mp4"] = "video/mp4";
	_types[".mpeg"] = "video/mpeg";
	_types[".mpg"] = "video/mpeg";
	_types[".mov"] = "video/quicktime";
	_types[".webm"] = "video/webm";
	_types[".flv"] = "video/x-flv";
	_types[".m4v"] = "video/x-m4v";
	_types[".mng"] = "video/x-mng";
	_types[".asx"] = "video/x-ms-asf";
	_types[".asf"] = "video/x-ms-asf";
	_types[".wmv"] = "video/x-ms-wmv";
	_types[".avi"] = "video/x-msvideo";
}

void Server::printDetails() const
{
	// "╔═╗╚═╝║";
	std::cout << "╔═══════════════════════════════════════╗" << std::endl;
	std::cout << "║    Server name : ";WIDTH(20);std::cout << _name          ;std::cout << " ║" << std::endl;
	std::cout << "║             IP : ";WIDTH(20);std::cout << _ip_addr       ;std::cout << " ║" << std::endl;
	std::cout << "║           Port : ";WIDTH(20);std::cout << _port          ;std::cout << " ║" << std::endl;
	std::cout << "║  Max Body Size : ";WIDTH(20);std::cout << _max_body_size ;std::cout << " ║" << std::endl;
	for (std::map<unsigned int, std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); ++it)
	{
	std::cout << "║ Error Page "<< it->first <<" : ";WIDTH(20);std::cout << it->second ;std::cout << " ║" << std::endl;
	}
	_root_loc->printDetails();
	std::cout << "╚═══════════════════════════════════════╝" << std::endl;
}

int Server::getSockFd() const
{
	return _sockfd;
}

Server& Server::operator=(const Server& copy)
{
	cgiHandler = copy.cgiHandler;
	_env = copy._env;
	_host = copy._host;
	_port = copy._port;
	_root_loc = new Location(*(copy._root_loc));
	_error_pages = copy._error_pages;
	return *this;
}

void Server::_setupServAddr()
{
	std::memset(&(_servaddr), 0, sizeof(_servaddr));
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = _host;
	_servaddr.sin_port = htons(_port);
}

void Server::setup()
{
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Failed to create socket");
	Logger::log("Socket created", DEBUG);
	
	int option_value = 1;
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, 
		&option_value, sizeof(int)) < 0)
		throw std::runtime_error("Failed to set socket options");
	
	_setupServAddr();
	
	std::ostringstream oss;
	oss << "Binding socket at " << _ip_addr << ":" << _port << ".";
	Logger::log(oss.str(), DEBUG);
	if (bind(_sockfd, (struct sockaddr *) &_servaddr, sizeof(_servaddr)) < 0)
		throw std::runtime_error("Failed to bind socket");
	
	Logger::log("Listening for connections", DEBUG);
	if (listen(_sockfd, 512) < 0)
		throw std::runtime_error("Failed to listen on socket");
	
	if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Failed to set socket in non-blocking mode");
	_ready = true;
	Logger::log("Server ready : " + getIp(), SUCCESS);
}

std::string Server::getIp()
{
	std::stringstream ss;

	ss << _ip_addr << ":" << _port;

	return ss.str();	
}

Location *Server::_getLocation(const std::string& path)
{
	return _root_loc->getSubLoc(path);
}

Response Server::executeRequest(Request& req)
{
	Response rep;

	if (req.body.size() > 0 && req.attributes["Content-Length"].size() == 0) {
		return errorPage(411);
	}

	_handleSession(req, rep);
	
	Location *loc = _getLocation(req.path);
	
	if (req.path.empty() || req.path == "/") {
		if (!loc->_index.empty()) {
			req.path = "/" + loc->_index;
			loc = _getLocation(req.path);
		}
	}
	
	std::string path = _findResourcePath(req, loc);
	
	req_type type = _getType(path);
	if (type == T_NFD) {
		return errorPage(404);
	}
	
	if (req.method != "GET" && req.method != "POST" && req.method != "DELETE") {
		return errorPage(405);
	}

	if (req.method == "DELETE") {
		return _delete(req);
	}

	if (type == T_FILE) {
		
		std::string originalPath = path;
		
		size_t queryPos = path.find("?");
		if (queryPos != std::string::npos) {
			path = path.substr(0, queryPos);
		}
		Logger::log("path : " + path, DEBUG);
		
		int access_result = access(path.c_str(), R_OK);
		Logger::log("access result: " + Utils::toString(access_result) + " (errno: " + Utils::toString(errno) + ")", DEBUG);
		
		if (access(path.c_str(), R_OK) != 0) {
			return errorPage(403);
		} else {
			path = originalPath;
			if (req.method == "POST" && req.attributes.find("Content-Type") == req.attributes.end()) {
				return errorPage(400);
			}

			if (!Utils::searchFor(loc->_allowed_methods, req.method)) {
				return errorPage(405);
			}

			try {
				if (cgiHandler->executeCGI(req, path, loc, rep)) {
					return rep;
				}
			} catch (const std::exception& e) {
				Logger::log("CGI execution failed: " + std::string(e.what()), ERROR);
				return errorPage(500);
			}
		}
	}

	Logger::log("Request method: " + req.method, DEBUG);
	Logger::log("Location: " + loc->getName(), DEBUG);
	Logger::log("Allowed methods: ", DEBUG);
	for (size_t i = 0; i < loc->_allowed_methods.size(); i++) {
		Logger::log(" - " + loc->_allowed_methods[i], DEBUG);
	}

	Logger::log("Not CGI, running methods...", DEBUG);
	if (req.method == "GET")
		return _get(req);
	if (req.method == "POST")
		return _post(req);
	
	return errorPage(500);
}

req_type Server::_getType(const std::string& path)
{
	std::string clean_path = path;
	size_t pos = clean_path.find("?");
	if (pos != std::string::npos) {
		clean_path = clean_path.substr(0, pos);
	}

	struct stat path_stat;
	if (stat(clean_path.c_str(), &path_stat) != 0)
		return T_NFD;
	if (S_ISDIR(path_stat.st_mode))
		return T_DIR;
	if (S_ISREG(path_stat.st_mode))
		return T_FILE;
	return T_NFD;
}

std::string Server::_findResourcePath(const Request &req, Location *loc)
{
	Location *l = loc;
	std::string root = l->_root;
	while (root.size() == 0 && l)
	{
		l = l->_parent;
		root = l->_root;
	}
	std::string path = root + req.path;
	std::vector<std::string> p = Utils::splitString(path, "/");
	if (p.size() == 0)
		return "/";
	path = p[0];
	for (size_t i = 1; i < p.size(); i++)
	{
		path += "/" + p[i];
	}
	return path;
}

Response Server::_get(const Request& req)
{
	Response rep;
	rep.http = "HTTP/1.1";

	try {
		Location *loc = _getLocation(req.path);
		if (!loc) {
			return errorPage(404);
		}

		if (!loc->_return.empty()) {
			std::vector<std::string> return_parts = Utils::splitString(loc->_return, " ");
			if (return_parts.size() >= 3 && return_parts[0] == "return") {
				rep.status = std::atoi(return_parts[1].c_str()); // redirect
				rep.attributes["Location"] = return_parts[2];
				rep.phrase = "Moved Permanently";
				rep.ready = true;
				return rep;
			}
		}

		if (!Utils::searchFor(loc->_allowed_methods, std::string("GET"))) {
			return errorPage(405);
		}

		std::string path = _findResourcePath(req, loc);
		req_type type = _getType(path);

		if (type == T_NFD) {
			return errorPage(404);
		}

		if (access(path.c_str(), R_OK) != 0 && type != T_NFD) {
			return errorPage(403);
		}

		if (type == T_FILE) {
			return _readFile(req, path);
		}
		else if (type == T_DIR) {
			if (req.path[req.path.size() - 1] != '/') {
				return errorPage(301);
			}

			if (!loc->_index.empty()) {
				std::string idx_path = path + "/" + loc->_index;
				if (_getType(idx_path) == T_FILE) {
					if (access(idx_path.c_str(), R_OK) != 0) {
						return errorPage(403);
					}
					return _readFile(req, idx_path);
				}
			}

			if (loc->_auto_index) {
				return _autoIndex(path);
			}
			return errorPage(403);
		}

		return errorPage(500);

	} catch(const std::exception& e) {
		Logger::log("GET error: " + std::string(e.what()), ERROR);
		return errorPage(500);
	}
}

std::string extractFilename(const std::string& input)
{
	size_t filenamePos = input.find("filename=\"");
	if (filenamePos != std::string::npos)
	{
		filenamePos += 10;
		size_t endPos = input.find("\"", filenamePos);
		if (endPos != std::string::npos)
			return input.substr(filenamePos, endPos - filenamePos);
	}
	return "";
}

Response Server::_post(const Request& req)
{
	Response rep;
	rep.http = "HTTP/1.1";
	
	try {
		Location *loc = _getLocation(req.path);
		if (!loc) {
			return errorPage(404);
		}

		if (!Utils::searchFor(loc->_allowed_methods, std::string("POST"))) {
			return errorPage(405);
		}

		if (req.body.size() > getMaxBodySize()) {
			return errorPage(413);
		}

		if (req.attributes.find("Content-Type") == req.attributes.end()) {
			return errorPage(400);
		}

		std::string path = _findResourcePath(req, loc);
		
		std::string parent_dir = path.substr(0, path.find_last_of("/"));
		if (!Utils::fileExists(parent_dir)) {
			return errorPage(409);
		}

		if (access(parent_dir.c_str(), W_OK) != 0) {
			return errorPage(403);
		}

		if (Utils::fileExists(path)) {
			std::ofstream file(path.c_str(), std::ios::binary);
			if (!file.is_open()) {
				return errorPage(500);
			}
			file.write(req.body.c_str(), req.body.size());
			file.close();
		} else {
			std::ofstream file(path.c_str(), std::ios::binary);
			if (!file.is_open()) {
				return errorPage(500);
			}
			file.write(req.body.c_str(), req.body.size());
			file.close();
		}

		rep.status = 201;
		rep.phrase = "Created";
		rep.body = "{\"status\": \"success\", \"message\": \"File uploaded successfully\"}";
		rep.attributes["Content-Type"] = "application/json";
		rep.attributes["Connection"] = "close";
		rep.ready = true;

	} catch (const std::exception& e) {
		Logger::log("POST error: " + std::string(e.what()), ERROR);
		return errorPage(500);
	}
	
	return rep;
}

Response Server::_delete(Request& req)
{
	Response rep;
	rep.http = "HTTP/1.1";
	
	try {
		Logger::log("DELETE request path: " + req.path, DEBUG);
		
		Location *loc = _getLocation(req.path);
		if (!loc) {
			Logger::log("Location not found for path: " + req.path, ERROR);
			return errorPage(404);
		}
		
		Logger::log("Allowed methods for location: ", DEBUG);
		for (size_t i = 0; i < loc->_allowed_methods.size(); i++) {
			Logger::log(" - " + loc->_allowed_methods[i], DEBUG);
		}

		if (!Utils::searchFor(loc->_allowed_methods, std::string("DELETE"))) {
			Logger::log("DELETE method not allowed for this location", ERROR);
			return errorPage(405);
		}

		std::string path = _findResourcePath(req, loc);
		Logger::log("Full resource path: " + path, DEBUG);
		
		if (!Utils::fileExists(path)) {
			Logger::log("File not found: " + path, ERROR);
			return errorPage(404);
		}

		struct stat path_stat;
		if (stat(path.c_str(), &path_stat) == 0) {
			if (S_ISDIR(path_stat.st_mode)) {
				Logger::log("Cannot delete directory: " + path, ERROR);
				return errorPage(403);
			}
		}

		Logger::log("Checking write permissions for: " + path, DEBUG);
		if (access(path.c_str(), W_OK) != 0) {
			Logger::log("No write permission for file: " + path + " (errno: " + Utils::toString(errno) + ")", ERROR);
			return errorPage(403);
		}

		Logger::log("Attempting to delete file: " + path, DEBUG);
		if (remove(path.c_str()) != 0) {
			Logger::log("Failed to delete file: " + path + " (errno: " + Utils::toString(errno) + ")", ERROR);
			return errorPage(500);
		}

		rep.status = 200;
		rep.phrase = "OK";
		rep.body = "{\"status\": \"success\", \"message\": \"File deleted successfully\"}";
		rep.attributes["Content-Type"] = "application/json";
		rep.attributes["Connection"] = "close";
		rep.ready = true;
		
	} catch (const std::exception& e) {
		Logger::log("Delete error: " + std::string(e.what()), ERROR);
		return errorPage(500);
	}
	
	return rep;
}

std::string Server::_listDirectory(const std::string &path)
{
	DIR *dir;
	struct dirent *ent;
	std::ostringstream html;

	dir = opendir(path.c_str());
	if (dir == NULL)
		return "";
		
	html << "<html><head><title>Index of " << path << "</title></head><body>";
	html << "<h1>Index of " << path << "</h1>";
	html << "<ul>";
		
	while ((ent = readdir(dir)) != NULL)
	{
		std::string name(ent->d_name);
		
		if (name == "." || name == "..") {
			continue;
		}

		std::string display_name = name;
		if (ent->d_type == DT_DIR) {
			display_name += "/";
		}
		
		html << "<li><a href=\"" << name << "\">" << display_name << "</a></li>";
	}
		
	html << "</ul>";
	html << "</body></html>";
		
	closedir(dir);
	return html.str();
}

Response Server::errorPage(unsigned int code)
{
    Response rep;
    std::string errorTitle;
    std::string errorDescription;
    std::string errorColor;
    
    // Set error details based on status code
    switch(code)
    {
        // 1xx Informational
        case 100:
            errorTitle = "Continue";
            errorDescription = "The server has received the request headers and the client should proceed to send the request body.";
            errorColor = "#3498DB";
            break;
        case 101:
            errorTitle = "Switching Protocols";
            errorDescription = "The requester has asked the server to switch protocols and the server has agreed to do so.";
            errorColor = "#3498DB";
            break;
        case 102:
            errorTitle = "Processing";
            errorDescription = "The server has received and is processing the request, but no response is available yet.";
            errorColor = "#3498DB";
            break;
        case 103:
            errorTitle = "Early Hints";
            errorDescription = "Used to return some response headers before final HTTP message.";
            errorColor = "#3498DB";
            break;

        // 2xx Success
        case 200:
            errorTitle = "OK";
            errorDescription = "The request has succeeded.";
            errorColor = "#2ECC71";
            break;
        case 201:
            errorTitle = "Created";
            errorDescription = "The request has been fulfilled and a new resource has been created.";
            errorColor = "#2ECC71";
            break;
        case 202:
            errorTitle = "Accepted";
            errorDescription = "The request has been accepted for processing, but the processing has not been completed.";
            errorColor = "#2ECC71";
            break;
        case 203:
            errorTitle = "Non-Authoritative Information";
            errorDescription = "The server is a transforming proxy that received a 200 OK from its origin, but is returning a modified version of the origin's response.";
            errorColor = "#2ECC71";
            break;
        case 204:
            errorTitle = "No Content";
            errorDescription = "The server successfully processed the request, but is not returning any content.";
            errorColor = "#2ECC71";
            break;
        case 205:
            errorTitle = "Reset Content";
            errorDescription = "The server successfully processed the request, but is not returning any content. The requester should reset the document view.";
            errorColor = "#2ECC71";
            break;
        case 206:
            errorTitle = "Partial Content";
            errorDescription = "The server is delivering only part of the resource due to a range header sent by the client.";
            errorColor = "#2ECC71";
            break;
        case 207:
            errorTitle = "Multi-Status";
            errorDescription = "The message body that follows is an XML message and can contain a number of separate response codes.";
            errorColor = "#2ECC71";
            break;
        case 208:
            errorTitle = "Already Reported";
            errorDescription = "The members of a DAV binding have already been enumerated in a previous reply to this request, and are not being included again.";
            errorColor = "#2ECC71";
            break;
        case 226:
            errorTitle = "IM Used";
            errorDescription = "The server has fulfilled a request for the resource, and the response is a representation of the result of one or more instance-manipulations applied to the current instance.";
            errorColor = "#2ECC71";
            break;

        // 3xx Redirection
        case 300:
            errorTitle = "Multiple Choices";
            errorDescription = "The requested resource has multiple representations available.";
            errorColor = "#F39C12";
            break;
        case 301:
            errorTitle = "Moved Permanently";
            errorDescription = "The requested resource has been permanently moved to a new URL.";
            errorColor = "#F39C12";
            break;
        case 302:
            errorTitle = "Found";
            errorDescription = "The requested resource temporarily resides under a different URL.";
            errorColor = "#F39C12";
            break;
        case 303:
            errorTitle = "See Other";
            errorDescription = "The response to the request can be found under a different URL.";
            errorColor = "#F39C12";
            break;
        case 304:
            errorTitle = "Not Modified";
            errorDescription = "The resource has not been modified since the last request.";
            errorColor = "#F39C12";
            break;
        case 305:
            errorTitle = "Use Proxy";
            errorDescription = "The requested resource must be accessed through the proxy given by the Location field.";
            errorColor = "#F39C12";
            break;
        case 307:
            errorTitle = "Temporary Redirect";
            errorDescription = "The requested resource temporarily resides under a different URL.";
            errorColor = "#F39C12";
            break;
        case 308:
            errorTitle = "Permanent Redirect";
            errorDescription = "The requested resource has been permanently moved to another URL.";
            errorColor = "#F39C12";
            break;

        // 4xx Client Error
        case 400:
            errorTitle = "Bad Request";
            errorDescription = "The server cannot process the request due to a client error.";
            errorColor = "#E74C3C";
            break;
        case 401:
            errorTitle = "Unauthorized";
            errorDescription = "Authentication is required and has failed or has not been provided.";
            errorColor = "#E74C3C";
            break;
        case 402:
            errorTitle = "Payment Required";
            errorDescription = "Reserved for future use. Payment is required to access this resource.";
            errorColor = "#E74C3C";
            break;
        case 403:
            errorTitle = "Forbidden";
            errorDescription = "You don't have permission to access this resource.";
            errorColor = "#E74C3C";
            break;
        case 404:
            errorTitle = "Not Found";
            errorDescription = "The requested resource could not be found on this server.";
            errorColor = "#E74C3C";
            break;
        case 405:
            errorTitle = "Method Not Allowed";
            errorDescription = "The request method is not supported for the requested resource.";
            errorColor = "#E74C3C";
            break;
        case 406:
            errorTitle = "Not Acceptable";
            errorDescription = "The requested resource is capable of generating only content not acceptable according to the Accept headers sent in the request.";
            errorColor = "#E74C3C";
            break;
        case 407:
            errorTitle = "Proxy Authentication Required";
            errorDescription = "Authentication with the proxy is required before proceeding.";
            errorColor = "#E74C3C";
            break;
        case 408:
            errorTitle = "Request Timeout";
            errorDescription = "The server timed out waiting for the request.";
            errorColor = "#E74C3C";
            break;
        case 409:
            errorTitle = "Conflict";
            errorDescription = "The request could not be completed due to a conflict with the current state of the resource.";
            errorColor = "#E74C3C";
            break;
        case 410:
            errorTitle = "Gone";
            errorDescription = "The requested resource is no longer available and will not be available again.";
            errorColor = "#E74C3C";
            break;
        case 411:
            errorTitle = "Length Required";
            errorDescription = "The request did not specify the length of its content, which is required by the requested resource.";
            errorColor = "#E74C3C";
            break;
        case 412:
            errorTitle = "Precondition Failed";
            errorDescription = "The server does not meet one of the preconditions that the requester put on the request.";
            errorColor = "#E74C3C";
            break;
        case 413:
            errorTitle = "Payload Too Large";
            errorDescription = "The request is larger than the server is willing or able to process.";
            errorColor = "#E74C3C";
            break;
        case 414:
            errorTitle = "URI Too Long";
            errorDescription = "The URI provided was too long for the server to process.";
            errorColor = "#E74C3C";
            break;
        case 415:
            errorTitle = "Unsupported Media Type";
            errorDescription = "The media format of the requested data is not supported by the server.";
            errorColor = "#E74C3C";
            break;
        case 416:
            errorTitle = "Range Not Satisfiable";
            errorDescription = "The client has asked for a portion of the file, but the server cannot supply that portion.";
            errorColor = "#E74C3C";
            break;
        case 417:
            errorTitle = "Expectation Failed";
            errorDescription = "The server cannot meet the requirements of the Expect request-header field.";
            errorColor = "#E74C3C";
            break;
        case 418:
            errorTitle = "I'm a teapot";
            errorDescription = "The server refuses to brew coffee because it is a teapot.";
            errorColor = "#E74C3C";
            break;
        case 421:
            errorTitle = "Misdirected Request";
            errorDescription = "The request was directed at a server that is not able to produce a response.";
            errorColor = "#E74C3C";
            break;
        case 422:
            errorTitle = "Unprocessable Entity";
            errorDescription = "The request was well-formed but was unable to be followed due to semantic errors.";
            errorColor = "#E74C3C";
            break;
        case 423:
            errorTitle = "Locked";
            errorDescription = "The resource that is being accessed is locked.";
            errorColor = "#E74C3C";
            break;
        case 424:
            errorTitle = "Failed Dependency";
            errorDescription = "The request failed due to failure of a previous request.";
            errorColor = "#E74C3C";
            break;
        case 425:
            errorTitle = "Too Early";
            errorDescription = "The server is unwilling to risk processing a request that might be replayed.";
            errorColor = "#E74C3C";
            break;
        case 426:
            errorTitle = "Upgrade Required";
            errorDescription = "The client should switch to a different protocol.";
            errorColor = "#E74C3C";
            break;
        case 428:
            errorTitle = "Precondition Required";
            errorDescription = "The origin server requires the request to be conditional.";
            errorColor = "#E74C3C";
            break;
        case 429:
            errorTitle = "Too Many Requests";
            errorDescription = "The user has sent too many requests in a given amount of time.";
            errorColor = "#E74C3C";
            break;
        case 431:
            errorTitle = "Request Header Fields Too Large";
            errorDescription = "The server is unwilling to process the request because its header fields are too large.";
            errorColor = "#E74C3C";
            break;
        case 451:
            errorTitle = "Unavailable For Legal Reasons";
            errorDescription = "The requested resource is unavailable due to legal reasons.";
            errorColor = "#E74C3C";
            break;

        // 5xx Server Error
        case 500:
            errorTitle = "Internal Server Error";
            errorDescription = "The server encountered an unexpected condition that prevented it from fulfilling the request.";
            errorColor = "#9B59B6";
            break;
        case 501:
            errorTitle = "Not Implemented";
            errorDescription = "The server does not support the functionality required to fulfill the request.";
            errorColor = "#9B59B6";
            break;
        case 502:
            errorTitle = "Bad Gateway";
            errorDescription = "The server received an invalid response from the upstream server.";
            errorColor = "#9B59B6";
            break;
        case 503:
            errorTitle = "Service Unavailable";
            errorDescription = "The server is currently unavailable (overloaded or down for maintenance).";
            errorColor = "#9B59B6";
            break;
        case 504:
            errorTitle = "Gateway Timeout";
            errorDescription = "The server was acting as a gateway and did not receive a timely response.";
            errorColor = "#9B59B6";
            break;
        case 505:
            errorTitle = "HTTP Version Not Supported";
            errorDescription = "The server does not support the HTTP protocol version used in the request.";
            errorColor = "#9B59B6";
            break;
        case 506:
            errorTitle = "Variant Also Negotiates";
            errorDescription = "Transparent content negotiation for the request results in a circular reference.";
            errorColor = "#9B59B6";
            break;
        case 507:
            errorTitle = "Insufficient Storage";
            errorDescription = "The server is unable to store the representation needed to complete the request.";
            errorColor = "#9B59B6";
            break;
        case 508:
            errorTitle = "Loop Detected";
            errorDescription = "The server detected an infinite loop while processing the request.";
            errorColor = "#9B59B6";
            break;
        case 510:
            errorTitle = "Not Extended";
            errorDescription = "Further extensions to the request are required for the server to fulfill it.";
            errorColor = "#9B59B6";
            break;
        case 511:
            errorTitle = "Network Authentication Required";
            errorDescription = "The client needs to authenticate to gain network access.";
            errorColor = "#9B59B6";
            break;
        
        // Unofficial or Non-Standard Codes (520+)
        case 520:
            errorTitle = "Web Server Returned an Unknown Error";
            errorDescription = "The origin server returned an unexpected response.";
            errorColor = "#95A5A6";
            break;
        case 521:
            errorTitle = "Web Server Is Down";
            errorDescription = "The origin server refused connections.";
            errorColor = "#95A5A6";
            break;
        case 522:
            errorTitle = "Connection Timed Out";
            errorDescription = "The connection to the origin server timed out.";
            errorColor = "#95A5A6";
            break;
        case 523:
            errorTitle = "Origin Is Unreachable";
            errorDescription = "The origin server is unreachable.";
            errorColor = "#95A5A6";
            break;
        case 524:
            errorTitle = "A Timeout Occurred";
            errorDescription = "A connection to the origin server was made, but the request timed out.";
            errorColor = "#95A5A6";
            break;
        case 525:
            errorTitle = "SSL Handshake Failed";
            errorDescription = "The SSL/TLS handshake failed.";
            errorColor = "#95A5A6";
            break;
        case 526:
            errorTitle = "Invalid SSL Certificate";
            errorDescription = "The origin server presented an invalid SSL certificate.";
            errorColor = "#95A5A6";
            break;
        case 527:
            errorTitle = "Railgun Error";
            errorDescription = "The request timed out or failed after the WAN connection was established.";
            errorColor = "#95A5A6";
            break;
        case 530:
            errorTitle = "Site is frozen";
            errorDescription = "This site has been frozen due to inactivity.";
            errorColor = "#95A5A6";
            break;
        
        default:
            if (code >= 600) {
                errorTitle = "Unknown Error";
                errorDescription = "An unknown error occurred while processing your request.";
                errorColor = "#95A5A6";
            } else if (code >= 500) {
                errorTitle = "Server Error";
                errorDescription = "The server encountered an error while processing your request.";
                errorColor = "#9B59B6";
            } else if (code >= 400) {
                errorTitle = "Client Error";
                errorDescription = "There was an error with your request.";
                errorColor = "#E74C3C";
            } else {
                errorTitle = "Unknown Status";
                errorDescription = "An unknown status code was returned.";
                errorColor = "#95A5A6";
            }
            break;
    }

    Logger::log(_error_pages[code], INFO);
    req_type type = _getType(_root_loc->_root + _error_pages[code]);
    if (type == T_FILE)
        rep.body = Utils::readFile(_root_loc->_root + _error_pages[code], true);
    else
    {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n";
        ss << "<html lang=\"en\">\n";
        ss << "<head>\n";
        ss << "    <meta charset=\"UTF-8\">\n";
        ss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        ss << "    <title>Error " << code << " - " << errorTitle << "</title>\n";
        ss << "    <style>\n";
        ss << "        * {\n";
        ss << "            margin: 0;\n";
        ss << "            padding: 0;\n";
        ss << "            box-sizing: border-box;\n";
        ss << "        }\n";
        ss << "        body {\n";
        ss << "            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;\n";
        ss << "            background-color: #f8f9fa;\n";
        ss << "            color: #333;\n";
        ss << "            display: flex;\n";
        ss << "            flex-direction: column;\n";
        ss << "            align-items: center;\n";
        ss << "            justify-content: center;\n";
        ss << "            min-height: 100vh;\n";
        ss << "            padding: 20px;\n";
        ss << "        }\n";
        ss << "        .error-container {\n";
        ss << "            background-color: white;\n";
        ss << "            border-radius: 12px;\n";
        ss << "            box-shadow: 0 10px 25px rgba(0,0,0,0.05);\n";
        ss << "            width: 100%;\n";
        ss << "            max-width: 600px;\n";
        ss << "            padding: 40px;\n";
        ss << "            text-align: center;\n";
        ss << "        }\n";
        ss << "        .error-code {\n";
        ss << "            font-size: 72px;\n";
        ss << "            font-weight: 700;\n";
        ss << "            color: " << errorColor << ";\n";
        ss << "            line-height: 1;\n";
        ss << "            margin-bottom: 10px;\n";
        ss << "        }\n";
        ss << "        .error-title {\n";
        ss << "            font-size: 24px;\n";
        ss << "            font-weight: 600;\n";
        ss << "            margin-bottom: 20px;\n";
        ss << "        }\n";
        ss << "        .error-description {\n";
        ss << "            color: #666;\n";
        ss << "            margin-bottom: 30px;\n";
        ss << "            line-height: 1.6;\n";
        ss << "        }\n";
        ss << "        .back-button {\n";
        ss << "            display: inline-block;\n";
        ss << "            background-color: " << errorColor << ";\n";
        ss << "            color: white;\n";
        ss << "            padding: 12px 24px;\n";
        ss << "            border-radius: 6px;\n";
        ss << "            text-decoration: none;\n";
        ss << "            font-weight: 500;\n";
        ss << "            transition: background-color 0.3s;\n";
        ss << "        }\n";
        ss << "        .back-button:hover {\n";
        ss << "            background-color: " << errorColor << "dd;\n";
        ss << "        }\n";
        ss << "        @media (max-width: 480px) {\n";
        ss << "            .error-code {\n";
        ss << "                font-size: 60px;\n";
        ss << "            }\n";
        ss << "            .error-title {\n";
        ss << "                font-size: 20px;\n";
        ss << "            }\n";
        ss << "            .error-container {\n";
        ss << "                padding: 30px 20px;\n";
        ss << "            }\n";
        ss << "        }\n";
        ss << "    </style>\n";
        ss << "</head>\n";
        ss << "<body>\n";
        ss << "    <div class=\"error-container\">\n";
        ss << "        <div class=\"error-code\">" << code << "</div>\n";
        ss << "        <h1 class=\"error-title\">" << errorTitle << "</h1>\n";
        ss << "        <p class=\"error-description\">" << errorDescription << "</p>\n";
        ss << "        <a href=\"/\" class=\"back-button\">Go Home</a>\n";
        ss << "    </div>\n";
        ss << "</body>\n";
        ss << "</html>";
        rep.body = ss.str();
    }
    rep.attributes["Content-Type"] = "text/html";
    rep.http = "HTTP/1.1";
    rep.status = code;
    rep.phrase = errorTitle;
    rep.attributes["Connection"] = "close";
    rep.ready = true;

    return rep;
}

Response Server::_autoIndex(const std::string& path)
{
	Response rep;
	rep.http = "HTTP/1.1";
	rep.status = 200;
	rep.phrase = "OK";
	rep.body = _listDirectory(path);
	rep.attributes["Content-Type"] = "text/html";
	rep.attributes["Connection"] = "close";
	rep.ready = true;
	return rep;
}

Response Server::_readFile(const Request& req, const std::string& path)
{
	Logger::log("Reading file", DEBUG);
	(void)req;

	_loadTypes();
	std::string content = Utils::readFile(path, true);
	Response rep;
	rep.body = content;
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos)
		rep.attributes["Content-Type"] = "text/html";
	else
	{
		std::string ext = &(path[pos]);
		if (_types.find(ext) != _types.end())
			rep.attributes["Content-Type"] = _types[ext];
		else
			rep.attributes["Content-Type"] = "application/octet-stream";
	}
	Logger::log("Content-Type : " + rep.attributes["Content-Type"], DEBUG);
	rep.http = "HTTP/1.1";
	rep.status = 200;
	rep.phrase = "OK";
	rep.ready = true;
	return rep;
}

size_t Server::getMaxBodySize()
{
	if (_max_body_size == 0)
		return 1024;
	return _max_body_size;
}

Server::~Server()
{
	delete cgiHandler;

	for (std::map<std::string, Session*>::iterator it = _sessions.begin(); 
		 it != _sessions.end(); ++it) {
		delete it->second;
	}
	_sessions.clear();
	
	if (_ready)
		close(_sockfd);
	if (_root_loc)
		delete _root_loc;
}

void Server::_handleSession(Request& req, Response& rep)
{
	_cleanExpiredSessions();

	std::string sessionId = "";
	bool hasCookie = false;
	if (req.attributes.find("Cookie") != req.attributes.end())
	{
		std::string cookies = req.attributes["Cookie"];
		size_t pos = cookies.find("PHPSESSID=");
		if (pos != std::string::npos)
		{
			pos += 10; // Longueur de "PHPSESSID="
			size_t end = cookies.find(";", pos);
			sessionId = cookies.substr(pos, end - pos);
			hasCookie = true;
		}
	}

	Session* session;
	if (sessionId.empty() || _sessions.find(sessionId) == _sessions.end())
	{
		session = new Session();
		if (_sessions[session->getId()])
			delete _sessions[session->getId()];
		_sessions[session->getId()] = session;
		if (!hasCookie)
			rep.attributes["Set-Cookie"] = "PHPSESSID=" + session->getId() + "; Path=/";
	}
	else
	{
		session = _sessions[sessionId];
		session->refresh();
	}
}

void Server::_cleanExpiredSessions()
{
	std::map<std::string, Session*>::iterator it = _sessions.begin();
	while (it != _sessions.end())
	{
		if (it->second->isExpired()) {
			delete it->second;
			_sessions.erase(it++);
		}
		else {
			++it;
		}
	}
}

bool Server::hasSession(const std::string& sessionId)
{
	return _sessions.find(sessionId) != _sessions.end();
}

Session* Server::getSession(const std::string& sessionId)
{
	if (hasSession(sessionId))
		return _sessions[sessionId];
	return NULL;
}

std::string Server::_getPath(Request& req)
{
	std::string path = req.path;
	if (path.find("?") != std::string::npos)
		path = path.substr(0, path.find("?"));
	if (path[0] == '/')
		path = path.substr(1);
	return path;
}

bool Server::sendResponse(Response&rep, int fd)
{
	std::string content = rep.build();
	Logger::log("Sending response", DEBUG);
	ssize_t len = content.size();
	ssize_t sent = send(fd, content.c_str(), len, MSG_NOSIGNAL);
	Logger::log("sent response", DEBUG);

	if (rep.attributes.find("Connection") != rep.attributes.end() && 
		rep.attributes["Connection"] == "close") {
		close(fd);
	}

	return sent == len;
}

void Server::runRequests()
{
	for (int i = 0; i < MAX_REQUESTS_PER_CYCLE; i++)
	{
		try {
			if (_reqs.size() == 0)
				return;
			Request *req = _reqs[0];
			_reqs.erase(_reqs.begin());

			Response rep = this->executeRequest(*req);
			req->client->addResponse(rep);
		} catch (...) {
			Logger::log("Failed to execute request", INFO);
		}
	}
}

void Server::runRequestsCli(Client *cli)
{
	for (int i = 0; i < MAX_REQUESTS_PER_CYCLE; i++)
	{
		try {
			if (_reqs.size() == 0)
				return;
			Request *req = _reqs[0];
			if (req->client != cli)
				return;
			_reqs.erase(_reqs.begin());

			Response rep = this->executeRequest(*req);
			
			req->client->addResponse(rep);
			delete req; // rends la memoire fdp
		} catch (...) {
			Logger::log("Failed to execute request", INFO);
		}
	}
}