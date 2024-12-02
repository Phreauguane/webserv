#include "Server.h"
#include "Utils.h"

Server::Server()
{
	//
}

Server::Server(const Server& copy) : ISessionManager(copy)
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
		
		if (strs[0] == "server" && i == 0) // ignore line
		{}
		else if (strs[0] == "location") // recursive location -> call Location(const string&)
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
		else if (strs[0] == "{" || strs[0] == "}")
		{}
		else
			_root_loc->parseLine(strs);
	}
}

Server::Server(const std::string& source, char **env)
{
	_env = env;
	cgiHandler = CGI(*this);
	cgiHandler.setup(_env);
	_root_loc = new Location(_env);
	_parseSource(source);
}

void Server::_loadTypes()
{
	_types[".html"] = "text/html";
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
	
	// Gérer la session avant d'exécuter la requête
	_handleSession(req, rep);
	
	// Récupérer la location visée par la requête
	Location *loc = _getLocation(req.path);
	
	// Si le chemin est vide ou "/", utiliser l'index configuré
	if (req.path.empty() || req.path == "/") {
		if (!loc->_index.empty())
		{
			// Modifier le chemin de la requête pour pointer vers l'index
			req.path = "/" + loc->_index;
			// Mettre à jour la location avec le nouveau chemin
			loc = _getLocation(req.path);
		}
	}
	
	std::string path = _findResourcePath(req, loc);
	
	// Tenter d'exécuter le CGI
	if (req.method == "DELETE")
		return _delete(req);
	else if (cgiHandler.executeCGI(req, path, loc, rep))
		return rep;
	Logger::log("Not CGI, running methods...", DEBUG);
	if (req.method == "GET")
		return _get(req);
	if (req.method == "POST")
		return _post(req);
	throw std::runtime_error("Unable to execute request");
}

req_type Server::_getType(const std::string& path)
{
	// Supprimer les paramètres GET de l'URL pour la vérification du type
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
	Location *loc = _getLocation(req.path);
	// verify method
	std::string method = "GET";
	Response rep;
	if (!Utils::searchFor(loc->_allowed_methods, method))
		return rep;
	std::string path = _findResourcePath(req, loc);
	try
	{
		req_type type = _getType(path);
		if (type == T_NFD)
			return _errorPage(404);
		else if (type == T_FILE)
			return _readFile(req, path);
		else if (req.path[req.path.size() - 1] != '/')
			return _errorPage(301);
		else if (loc->_index.size() > 0)
		{
			std::string idx_path = path + "/" + loc->_index;
			if (_getType(idx_path) != T_FILE)
				return (loc->_auto_index ? _autoIndex(path) : _errorPage(403));
			return _readFile(req, idx_path);
		}
		else if (loc->_auto_index)
			return _autoIndex(path);
	}
	catch(const std::runtime_error& e)
	{
		Logger::log(e.what(), ERROR);
	}
	return rep;
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
		// Vérification de la location
		Location *loc = _getLocation(req.path);
		if (!loc) {
			rep.status = 404;
			rep.phrase = "Not Found";
			rep.body = "Location not found";
			rep.ready = true;
			return rep;
		}

		// Vérification des permissions de méthode
		std::string method = "POST";
		if (!Utils::searchFor(loc->_allowed_methods, method)) {
			rep.status = 405;
			rep.phrase = "Method Not Allowed";
			rep.body = "POST method not allowed for this location";
			rep.attributes["Allow"] = "GET, DELETE";
			rep.ready = true;
			return rep;
		}

		// Vérification de la taille du body
		if (req.body.size() > getMaxBodySize()) {
			rep.status = 413;
			rep.phrase = "Payload Too Large";
			rep.body = "Request entity too large";
			rep.ready = true;
			return rep;
		}

		// Vérification du Content-Type
		if (req.attributes.find("Content-Type") == req.attributes.end()) {
			rep.status = 400;
			rep.phrase = "Bad Request";
			rep.body = "Content-Type header is required";
			rep.ready = true;
			return rep;
		}

		// Construction du chemin complet
		std::string path = _findResourcePath(req, loc);
		
		// Vérification du dossier parent
		std::string parent_dir = path.substr(0, path.find_last_of("/"));
		if (!Utils::fileExists(parent_dir)) {
			rep.status = 409;
			rep.phrase = "Conflict";
			rep.body = "Parent directory does not exist";
			rep.ready = true;
			return rep;
		}

		// Vérification des permissions d'écriture
		if (access(parent_dir.c_str(), W_OK) != 0) {
			rep.status = 403;
			rep.phrase = "Forbidden";
			rep.body = "Permission denied";
			rep.ready = true;
			return rep;
		}

		// Traitement du fichier
		if (Utils::fileExists(path)) {
			// Le fichier existe déjà, on le met à jour
			std::ofstream file(path.c_str(), std::ios::binary);
			if (!file.is_open()) {
				rep.status = 500;
				rep.phrase = "Internal Server Error";
				rep.body = "Failed to open file for writing";
				rep.ready = true;
				return rep;
			}
			file.write(req.body.c_str(), req.body.size());
			file.close();
		} else {
			// Création d'un nouveau fichier
			std::ofstream file(path.c_str(), std::ios::binary);
			if (!file.is_open()) {
				rep.status = 500;
				rep.phrase = "Internal Server Error";
				rep.body = "Failed to create file";
				rep.ready = true;
				return rep;
			}
			file.write(req.body.c_str(), req.body.size());
			file.close();
		}

		// Succès
		rep.status = 201;
		rep.phrase = "Created";
		rep.body = "{\"status\": \"success\", \"message\": \"File uploaded successfully\"}";
		rep.attributes["Content-Type"] = "application/json";
		rep.ready = true;

	} catch (const std::exception& e) {
		Logger::log("POST error: " + std::string(e.what()), ERROR);
		rep.status = 500;
		rep.phrase = "Internal Server Error";
		rep.body = "An unexpected error occurred";
		rep.ready = true;
	}
	
	return rep;
}

Response Server::_delete(Request& req)
{
	Response rep;
	rep.http = "HTTP/1.1";
	
	try {
		// Vérification de la location
		Location *loc = _getLocation(req.path);
		if (!loc) {
			rep.status = 404;
			rep.phrase = "Not Found";
			rep.body = "Location not found";
			rep.ready = true;
			return rep;
		}

		// Vérification des permissions de méthode
		std::string method = "DELETE";
		if (!Utils::searchFor(loc->_allowed_methods, method)) {
			rep.status = 405;
			rep.phrase = "Method Not Allowed";
			rep.body = "DELETE method not allowed for this location";
			rep.attributes["Allow"] = "GET, POST";
			rep.ready = true;
			return rep;
		}

		// Construction du chemin complet
		std::string path = _findResourcePath(req, loc);
		
		// Vérification de l'existence du fichier
		if (!Utils::fileExists(path)) {
			rep.status = 404;
			rep.phrase = "Not Found";
			rep.body = "File not found";
			rep.ready = true;
			return rep;
		}

		// Vérification du type (ne pas supprimer les dossiers)
		struct stat path_stat;
		if (stat(path.c_str(), &path_stat) == 0) {
			if (S_ISDIR(path_stat.st_mode)) {
				rep.status = 403;
				rep.phrase = "Forbidden";
				rep.body = "Cannot delete directories";
				rep.ready = true;
				return rep;
			}
		}

		// Vérification des permissions
		if (access(path.c_str(), W_OK) != 0) {
			rep.status = 403;
			rep.phrase = "Forbidden";
			rep.body = "Permission denied";
			rep.ready = true;
			return rep;
		}

		// Tentative de suppression
		if (remove(path.c_str()) != 0) {
			rep.status = 500;
			rep.phrase = "Internal Server Error";
			rep.body = "Failed to delete file";
			rep.ready = true;
			return rep;
		}

		// Succès
		rep.status = 200;
		rep.phrase = "OK";
		rep.body = "{\"status\": \"success\", \"message\": \"File deleted successfully\"}";
		rep.attributes["Content-Type"] = "application/json";
		rep.ready = true;
		
	} catch (const std::exception& e) {
		Logger::log("Delete error: " + std::string(e.what()), ERROR);
		rep.status = 500;
		rep.phrase = "Internal Server Error";
		rep.body = "An unexpected error occurred";
		rep.ready = true;
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
		return "";  // If directory doesn't exist, return a 404 error
		
	html << "<html><head><title>Index of " << path << "</title></head><body>";
	html << "<h1>Index of " << path << "</h1>";
	html << "<ul>";
		
	while ((ent = readdir(dir)) != NULL)
	{
		std::string name(ent->d_name);
		
		// Ignore "." and ".."
		if (name == "." || name == "..") {
			continue;
		}

		// Add a trailing slash if it's a directory
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

Response Server::_errorPage(unsigned int code)
{
	Response rep;

	Logger::log(_error_pages[code], INFO);
	req_type type = _getType(_root_loc->_root + _error_pages[code]);
	if (type == T_FILE)
		rep.body = Utils::readFile(_root_loc->_root + _error_pages[code]);
	else
	{
		std::stringstream ss;
		ss << "<html><head><title>Error " << code << "</title></head>";
		ss << "<body><h1>Error " << code << "</h1></body></html>";
		rep.body = ss.str();
	}
	rep.attributes["Content-Type"] = "text/html";
	rep.http = "HTTP/1.1";
	rep.status = code;
	rep.phrase = "ERROR";
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
	rep.ready = true;
	return rep;
}

Response Server::_readFile(const Request& req, const std::string& path)
{
	Logger::log("Reading file", DEBUG);
	(void)req;

	// If execution fails or if file is not .php
	std::string content = Utils::readFile(path);
	Response rep;
	rep.body = content;
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos)
		rep.attributes["Content-Type"] = "text/html";
	else
	{
		std::string ext = &(path[pos]);
		if (ext == ".html")
			rep.attributes["Content-Type"] = "text/html";
		else if (ext == ".css")
			rep.attributes["Content-Type"] = "text/css";
		else if (ext == ".js")
		{
			Logger::log("JS file", DEBUG);
			rep.attributes["Content-Type"] = "application/javascript";
		}
		else
			rep.attributes["Content-Type"] = ext;
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
	// Nettoyer toutes les sessions
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
	// Nettoyer les sessions expirées
	_cleanExpiredSessions();

	// Chercher le cookie de session
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

	// Créer ou récupérer la session
	Session* session;
	if (sessionId.empty() || _sessions.find(sessionId) == _sessions.end())
	{
		session = new Session();
		_sessions[session->getId()] = session;
		// Set cookie seulement si aucun cookie n'existe déjà
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
