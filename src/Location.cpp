/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 00:05:31 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/30 12:22:23 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.h"
#include "Utils.h"

Location::Location()
{
	//
}

Location::Location(char **env)
{
	_env = env;
}

Location::Location(const Location& copy)
{
	*this = copy;
}

Location::Location(const std::string& source, char **env)
{
	_env = env;
	parseConfig(source);	
}

void Location::printDetails() const
{
	std::string methods;
	std::string cgi_paths;
	std::string cgi_exts;
	for (size_t i = 0; i < _allowed_methods.size(); i++)
	{
		methods += " " + _allowed_methods[i];
	}
	for (size_t i = 0; i < _cgi_paths.size(); i++)
	{
		cgi_paths += " " + _cgi_paths[i];
	}
	for (size_t i = 0; i < _cgi_exts.size(); i++)
	{
		cgi_exts += " " + _cgi_exts[i];
	}

	std::cout << "╠╦══════════════════════════════════════╣" << std::endl;
	std::cout << "║║      LOCATION : ";WIDTH(20);std::cout << _name          ;std::cout << " ║" << std::endl;
	std::cout << "║║          root : ";WIDTH(20);std::cout << _root          ;std::cout << " ║" << std::endl;
	std::cout << "║║         index : ";WIDTH(20);std::cout << _index         ;std::cout << " ║" << std::endl;
	std::cout << "║║    Auto Index : ";WIDTH(20);std::cout << (_auto_index ? "ON" : "OFF") ;std::cout << " ║" << std::endl;
	std::cout << "║║ Allow methods : ";WIDTH(20);std::cout << methods        ;std::cout << " ║" << std::endl;
	std::cout << "║║     cgi paths : ";WIDTH(20);std::cout << cgi_paths      ;std::cout << " ║" << std::endl;
	std::cout << "║║      cgi exts : ";WIDTH(20);std::cout << cgi_exts       ;std::cout << " ║" << std::endl;
	std::cout << "╠╩══════════════════════════════════════╣" << std::endl;
	for (std::map<std::string, Location*>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		std::cout << "╠  Sub-location  ═══════════════════════╣" << std::endl;
		it->second->printDetails();
	}
}

std::string Location::getName() const
{
	return _name;
}

void Location::parseLine(const std::vector<std::string>& strs)
{
	if (strs[0] == "root")
	{
		Utils::verify_args(strs, 2, 2);
		_root = strs[1];
	}
	else if (strs[0] == "alias")
	{
		Utils::verify_args(strs, 2, 2);
		_alias = strs[1];
	}
	else if (strs[0] == "index")
	{
		Utils::verify_args(strs, 2, 2);
		_index = strs[1];
	}
	else if (strs[0] == "allow_methods")
	{
		Utils::verify_args(strs, 2, 999);
		for (size_t i = 1; i < strs.size(); i++)
		{
			_allowed_methods.push_back(strs[i]);
		}
	}
    else if (strs[0] == "cgi_path") 
	{
		Utils::verify_args(strs, 2, 999);
		for (size_t i = 1; i < strs.size(); i++)
		{
			_cgi_paths.push_back(strs[i]);
		}
    }
    else if (strs[0] == "cgi_ext") 
	{
		Utils::verify_args(strs, 2, 999);
		for (size_t i = 1; i < strs.size(); i++)
		{
			_cgi_exts.push_back(strs[i]);
		}
    }
	else if (strs[0] == "autoindex")
	{
		Utils::verify_args(strs, 2, 2);
        _auto_index = (strs[1] == "on" || strs[1] == "ON");
    }
	else if (strs[0] == "return")
	{
		Utils::verify_args(strs, 2, 3);
		_return = "return";
		for (size_t i = 1; i < strs.size(); i++)
		{
			_return += " " + strs[i];
		}
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
		throw std::runtime_error("Unkown identifier : " + line);
	}
	else
		throw std::runtime_error("Unhandled parsing error");
}

void Location::parseConfig(const std::string& configString) {
	
	size_t position = 0;
	std::vector<std::string> lines = Utils::splitString(configString, "\n");
	
    for (size_t i = 0; i < lines.size(); ++i) {
		position = configString.find(lines[i], position + 1);
		std::string line = Utils::removeSemicolon(Utils::removeWSpaces(lines[i]));
		std::vector<std::string> strs = Utils::splitString(line, " ");
		
		if (strs[0] == "location" && i == 0) // main location -> store name
		{
			size_t start = strs[1].find("/");
			if (start == std::string::npos)
				_name = strs[1];
			else
				_name = strs[1].substr(start + 1, strs[1].size() - (start + 1));
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
	Location* loc = new Location(source, _env);
	loc->_parent = this;
	_locations[loc->_name] = loc;
}

Location *Location::getSubLoc(const std::string &path)
{
	if (path.size() == 0 || path == "/" || path == _name || path == "/" + _name)
		return this;
	
	std::vector<std::string> names = Utils::splitString(path, "/");
	
	std::string subpath;
	if (names.size() > 1)
	{
		size_t pos = path.find(names[1]);
		subpath = path.substr(pos, path.size() - pos);
	}
	else if (names.size() == 1)
	{
		subpath = "";
	}
	
	for (std::map<std::string, Location*>::const_iterator it = _locations.begin(); it != _locations.end(); ++it)
	{
		if (names[0] == it->second->getName())
			return it->second->getSubLoc(subpath);
	}
	return this;
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
	_env = copy._env;
	_name = copy._name;
	_root = copy._root;
	_index = copy._index;
	_auto_index = copy._auto_index;
	_allowed_methods = copy._allowed_methods;
	for(std::map<std::string, Location*>::const_iterator it = copy._locations.begin(); it != copy._locations.end(); ++it)
	{
		Location* loc = new Location(*(it->second));
		loc->_parent = this;
		_locations[loc->_name] = loc;
	}
	return *this;
}