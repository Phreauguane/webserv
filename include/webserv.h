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
#include <climits>

#include <fstream>
#include <sstream>

#include <string>
#include <map>
#include <vector>
#include <algorithm>

#define MAX_EVENTS 512
#define BUFFER_SIZE 30720
#define MAX_REQUESTS 1024
#define MAX_REQUESTS_PER_CYCLE 1
#define DEFAULT_TIMEOUT 30000

#ifndef LOG_LEVEL
# define LOG_LEVEL INFO
#endif

#include "Logger.h"
