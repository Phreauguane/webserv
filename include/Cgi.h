/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 15:28:31 by rmidou            #+#    #+#             */
/*   Updated: 2024/10/11 23:33:04 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include "Request.h"
#include "Response.h"

namespace Cgi
{
	std::string executePHP(const std::string& scriptPath, const std::string& query);
	bool executeCGI(const Request&, Response&);
}