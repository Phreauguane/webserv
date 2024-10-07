/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmidou <rmidou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 15:28:31 by rmidou            #+#    #+#             */
/*   Updated: 2024/10/07 15:49:20 by rmidou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.h"
#include <sys/wait.h>

namespace Cgi
{
	std::string executePHP(const std::string& scriptPath, const std::string& query);
}