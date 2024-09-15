/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 00:05:31 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/15 18:41:33 by jde-meo          ###   ########.fr       */
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
	size_t i0 = source.find('/');
	size_t i1 = source.find('{');
	if (i1 < i0 || i0 == std::string::npos)
		throw "parsing error in location : \n" + source + "\n: no name found !";
	i1 = source.find(' ', i0);
	if (i1 == std::string::npos)
		i1 = source.find('{', i0);
	if (i1 == std::string::npos)
		throw "parsing error in location : \n" + source + "\n: no brackets found !";
	_name = source.substr(i0, i1 - i0);
	
	i0 = source.find('{', i0);
	
	
}

void Location::setName(const std::string& name)
{
	_name = name;
}

void Location::setRoot(const std::string& root)
{
	_root = root;
}

void Location::setIndex(const std::string& index)
{
	_index = index;
}

void Location::setAutoIndex(bool auto_index)
{
	_auto_index = auto_index;
}

void Location::addMethod(const std::string& method)
{
	_methods.push_back(method);
}

void Location::addChildren(const std::string& source)
{
	_children.push_back(new Location(source));
}

Location::~Location()
{
	// Default destructor (still no idea of what's needed here for now)
}

Location& Location::operator=(const Location& copy)
{
	_name = copy._name;
	_root = copy._root;
	_index = copy._index;
	_auto_index = copy._auto_index;
	_methods = copy._methods;
	for (size_t i = 0; i < copy._children.size(); i++)
	{
		_children.push_back(new Location(*(copy._children[i])));
	}
	return *this;
}