/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 00:05:31 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/17 17:07:51 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.h"
#include "Utils.h"

Location::Location()
{
	// Default constructor (no idea what's needed whatsoever)
}

Location::Location(const Location& copy)
{
	*this = copy;
}

Location::Location(const std::string& source)
{
	_parseConfig(source);	
}

std::string Location::getName() const
{
	return _name;
}

void Location::parseLine(const std::vector<std::string>& strs)
{
	if (strs[0] == "root")
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
        _auto_index = (strs[1] == "on" || strs[1] == "ON");
		std::cout << "AUTO INDEX : " << (_auto_index ? "ON" : "OFF") << std::endl;
    }
	else if (strs[0] == "{" || strs[0] == "}")
	{}
	else if (strs.size() > 0)
	{
		std::string line = strs[0];
		for (size_t i = 1; i < strs.size(); i++)
		{
			line += " " + strs[i];
		}
		throw "Unkown identifier : " + line;
	}
}

void Location::_parseConfig(const std::string& configString) {
    // std::istringstream stream(configString);
	size_t position = 0;
	std::vector<std::string> lines = Utils::splitString(configString, "\n");
	
    for (size_t i = 0; i < lines.size(); ++i) {
		position = configString.find(lines[i], position + 1);
		std::string line = Utils::removeSemicolon(Utils::removeWSpaces(lines[i]));
		std::vector<std::string> strs = Utils::splitString(line, " ");
		
		if (strs[0] == "location" && i == 0) // main location -> store name
		{
			_name = strs[1];
			std::cout << "NAME : " << _name << std::endl;
		}
		else if (strs[0] == "location") // recursive location -> call Location(const string&)
		{
			std::string source = Utils::readBrackets(configString, position);
			std::vector<std::string> temp = Utils::splitString(source, "\n");
			this->addChildren(source);
			i += temp.size() - 1;
			continue;
		}
		else
			this->parseLine(strs);
    }
}

void Location::addMethod(const std::string& method)
{
	_allowed_methods.push_back(method);
}

void Location::addChildren(const std::string& source)
{
	Location* loc = new Location(source);
	_locations[loc->_name] = loc;
}

Location::~Location()
{
	for(std::map<std::string, Location*>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		delete it->second;
	}
}

Location& Location::operator=(const Location& copy)
{
	_name = copy._name;
	_root = copy._root;
	_index = copy._index;
	_auto_index = copy._auto_index;
	_allowed_methods = copy._allowed_methods;
	for(std::map<std::string, Location*>::const_iterator it = copy._locations.begin(); it != copy._locations.end(); ++it)
	{
		Location* loc = new Location(*(it->second));
		_locations[loc->_name] = loc;
	}
	return *this;
}