/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 15:52:52 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/27 15:56:19 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"

class Response
{
public:
	Response();
	Response(const Response&);
	Response &operator=(const Response&);
	std::string build();
public:
	std::string http;
	int status;
	std::string phrase;
	std::string content_type;
	std::string body;
};