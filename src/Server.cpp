/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:19:33 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/17 17:08:18 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.h"
#include "Utils.h"

// calls the constructor with an empty config as parameter
Server::Server()
{
	// to do
}

Server::Server(const Server& copy)
{
	*this = copy;
}

void Server::_parseSource(const std::string& source)
{// std::istringstream stream(configString);
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
			std::string source = Utils::readBrackets(source, position);
			std::vector<std::string> temp = Utils::splitString(source, "\n");
			Location* loc = new Location(source);
			if (loc->getName() == "/")
			{
				
			}
			i += temp.size() - 1;
			continue;
		}
		else if (strs[0] == "root")
		{
			_root = strs[1];
			std::cout << "ROOT : " << _root << std::endl;
		}
		else if (strs[0] == "alias")
		{
			_alias = strs[1];
			std::cout << "ALIAS : " << _alias << std::endl;
		}
		else if (strs[0] == "index")
		{
			_index = strs[1];
			std::cout << "INDEX : " << _index << std::endl;
		}
		else if (strs[0] == "allow_methods")
		{
			std::cout << "ALLOWED METHODS : ";
			for (size_t i = 1; i < strs.size(); i++)
			{
				_allowed_methods.push_back(strs[i]);
				std::cout << strs[i] << " ";
			}
			std::cout << std::endl;
		}
        else if (strs[0] == "cgi_path") 
		{
			std::cout << "CGI PATH : ";
			for (size_t i = 1; i < strs.size(); i++)
			{
				_cgi_paths.push_back(strs[i]);
				std::cout << strs[i] << " ";
			}
			std::cout << std::endl;
        }
        else if (strs[0] == "cgi_ext") 
		{
			std::cout << "CGI EXT : ";
			for (size_t i = 1; i < strs.size(); i++)
			{
				_cgi_paths.push_back(strs[i]);
				std::cout << strs[i] << " ";
			}
			std::cout << std::endl;
        }
		else if (strs[0] == "autoindex")
		{
            setAutoIndex(strs[1] == "on" || strs[1] == "ON");
			std::cout << "AUTO INDEX : " << (_auto_index ? "ON" : "OFF") << std::endl;
        }
		else if (strs[0] == "{" || strs[0] == "}")
		{}
		else if (line.size() > 0)
			throw "Unkown identifier : " + line;
        // Handle other potential fields...
    }
}

Server::Server(const std::string& source)
{
	_root_loc = new Location();
	/*
		Server constructor, takes source as parameter (string containing "server {...}")
		builds a Server object based on everything specified inside
		should handle synthax and parsing errors (NEEDS TO BE BULLETPROOF)

		For now only prints the source code given as parameter

		Final constructor to be done
	*/

	// print the source code with a simple header for readability
	std::cout << "> Server constructor called <" << std::endl;
	std::cout << ">          START            <" << std::endl;
	std::cout <<             source              << std::endl;
	std::cout << ">           END             <" << std::endl;
}

Server::~Server()
{
	if (_root_loc)
		delete _root_loc;
}

Server& Server::operator=(const Server& copy)
{
	_host = copy._host;
	_port = copy._port;
	_root_loc = new Location(*(copy._root_loc));
	_error_pages = copy._error_pages;
	return *this;
}