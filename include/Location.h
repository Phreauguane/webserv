#pragma once

#include "webserv.h"

class Location
{
public:
	Location();
	Location(const std::string&, char**);
	Location(const Location&);
	Location(const std::string&, const std::string&, char**);
	std::string getName() const;
	std::string getFullPath() const;
	void printDetails() const;
	void parseLine(const std::vector<std::string>&);
	void parseConfig(const std::string&);
	void addMethod(const std::string&);
	void addChildren(const std::string&);
	Location *getSubLoc(const std::string&);
	Location& operator=(const Location&);
	~Location();
public:
	char **_env;
	std::string _filename;
	std::string _name, _root, _alias, _index, _return, _upload;
	bool _auto_index;
	std::vector<std::string> _allowed_methods;
	std::vector<std::string> _cgi_exts;
	Location *_parent;
	std::map<std::string, Location*> _locations;
};
