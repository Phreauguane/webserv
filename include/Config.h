/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:29 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/19 19:18:23 by jde-meo          ###   ########.fr       */
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
	void		_createServers();
	std::string _source;
	std::vector<Server*> _servers;
};