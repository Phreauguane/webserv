/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:29 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/15 14:09:55 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Server.h"

class Config
{
public:
	Config();
	Config(const Config&);
	Config(const std::string&);
	Config& operator=(const Config&);
	virtual ~Config();
private:
	std::string _readFile(const std::string&);
	void		_createServers();
	std::string _source;
	std::vector<Server*> _servers;
};