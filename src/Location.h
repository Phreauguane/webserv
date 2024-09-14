/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 16:23:29 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/14 12:48:47 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"

class Location
{
public:
	Location();
	Location(const Location&);
	virtual ~Location();
private:
	std::string _name, _root, _index;
	bool _auto_index;
	std::vector<std::string> _methods;
	std::vector<Location> _children;
};