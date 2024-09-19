/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 17:43:11 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/19 19:36:02 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <fcntl.h>
#include <fstream>
#include <sstream>

#define DEF "\033[0m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_PURPLE "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"
#define BOLD_WHITE "\033[1;37m"

enum log_level{
	DEBUG = 0,
	INFO = 1,
	SUCCESS = 2,
	WARNING = 3,
	ERROR = 4
};

namespace Logger
{
	extern std::fstream log_file;
	extern log_level min_lvl;
	
	void log(const std::string&, const log_level);
	void setMinLogLevel(log_level);
	void setLogFile(const std::string&);
	void saveLog();
}