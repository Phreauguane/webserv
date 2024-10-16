/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/13 14:36:58 by jde-meo           #+#    #+#             */
/*   Updated: 2024/10/11 23:32:04 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <csignal>

#include <fstream>
#include <sstream>

#include <string>
#include <map>
#include <vector>
#include <algorithm>

#define MAX_EVENTS 512

#ifndef LOG_LEVEL
# define LOG_LEVEL INFO
#endif

#include "Logger.h"