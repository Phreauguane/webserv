/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 16:23:29 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/15 14:30:59 by jde-meo          ###   ########.fr       */
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
	void setName(const std::string&);
	void setRoot(const std::string&);
	void setIndex(const std::string&);
	void setAutoIndex(bool);
	void addMethod(const std::string&);
	void addChildren(const std::string&);
	Location& operator=(const Location&);
	virtual ~Location();
private:
	std::string _name, _root, _index;
	bool _auto_index;
	std::vector<std::string> _methods;
	std::vector<Location*> _children;
};