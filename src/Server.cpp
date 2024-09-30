/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:19:33 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/30 12:57:23 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        // Handle other potential fields...
    }
}

Server::Server(const std::string& source, char **env)
{
	_env = env;
	_root_loc = new Location(_env);
	_parseSource(source);
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
	
	_setupServAddr();
	
	int option_value = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, 
		&option_value, sizeof(int)) < 0)
		throw std::runtime_error("Failed to set socket options");
	
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
	Logger::log("Server " + getIp() + " OK", DEBUG);
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

Response Server::executeRequest(const Request& req)
{
	if (req.method == "GET")
		return _get(req);
	if (req.method == "POST")
		return _post(req);
	if (req.method == "DELETE")
		return _delete(req);
	throw std::runtime_error("Unkown method : " + req.method);
}

req_type Server::_getType(const std::string& path)
{
    struct stat path_stat;
    if (stat(path.c_str(), &path_stat) != 0)
		throw std::runtime_error("Invalid resource : " + path);
    if (S_ISDIR(path_stat.st_mode))
		return T_DIR;
	return T_FILE;
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
	std::string path = _findResourcePath(req, loc);
	try
	{
		if (_getType(path) == T_FILE)
			return _readFile(req, path);
		else if (loc->_index.size() > 0)
			return _readFile(req, path + "/" + loc->_index);
		else if (loc->_auto_index)
			return _autoIndex(path, loc);
	}
	catch(const std::runtime_error& e)
	{
		Logger::log(e.what(), ERROR);
	}
	Response rep;
	return rep;
}

Response Server::_autoIndex(const std::string& path, Location *loc)
{
	
}

Response Server::_readFile(const Request& req, const std::string& path)
{
	(void)req;
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
		else
			rep.attributes["Content-Type"] = ext;
	}
	rep.http = "HTTP/1.1";
	rep.status = 200;
	rep.phrase = "OK";
	rep.ready = true;
	return rep;
}

Response Server::_post(const Request& req)
{
	(void)req;
	Response rep;
	return rep;
}

Response Server::_delete(const Request& req)
{
	(void)req;
	Response rep;
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
	if (_ready)
		close(_sockfd);
	if (_root_loc)
		delete _root_loc;
}
