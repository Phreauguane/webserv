/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 00:25:35 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/19 19:18:11 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.h"

#define WIDTH(x) std::cout.width(x)

namespace Utils
{
	std::string removeComments(const std::string&);
	std::string readBrackets(const std::string&, size_t);
	std::string removeWSpaces(const std::string&);
	std::string removeSemicolon(const std::string&);
	std::string getCurrentTime();
	std::string readFile(const std::string&);
	std::vector<std::string> splitString(const std::string&, const std::string&);
	int inet_pton_v4(const std::string&, in_addr*);
	void verify_args(const std::vector<std::string>&, size_t, size_t);
}