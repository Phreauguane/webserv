/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 16:23:29 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/17 17:07:25 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"

class Location
{
public:
	Location();
	Location(const Location&);
	Location(const std::string&);
	std::string getName() const;
	void parseLine(const std::vector<std::string>&);
	void addMethod(const std::string&);
	void addChildren(const std::string&);
	Location& operator=(const Location&);
	virtual ~Location();
private:
	std::string _name, _root, _alias, _index;
	bool _auto_index;
	std::vector<std::string> _allowed_methods;
	std::vector<std::string> _cgi_paths;
	std::map<std::string, Location*> _locations;

	void _parseConfig(const std::string&);
};