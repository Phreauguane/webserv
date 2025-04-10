#pragma once

#include "webserv.h"
#include <sys/stat.h>

#define WIDTH(x) std::cout.width(x)

namespace Utils
{
	std::string removeComments(const std::string&);
	std::string readBrackets(const std::string&, size_t);
	std::string removeWSpaces(const std::string&);
	std::string removeSemicolon(const std::string&);
	std::string getCurrentTime();
	std::string readFile(const std::string& filename, bool useEpoll = false);
	std::string readFD(int fd, bool isEpollManaged = false);
	std::string toString(int);
	std::string toString(float);
	std::string toString(size_t);
	std::string toString(ssize_t);
	std::vector<std::string> splitString(const std::string&, const std::string&);
	int inet_pton_v4(const std::string&, in_addr_t*);
	void verify_args(const std::vector<std::string>&, const std::string&, size_t, size_t);
	template <typename T>
	bool searchFor(const std::vector<T>& vec, const T& e) {
		return std::find(vec.begin(), vec.end(), e) != vec.end();
	}
	std::string getExt(const std::string&);
	bool uploadFiles(std::string files, std::string path, std::string fileName);
	std::string generateRandomString(size_t length);
	bool fileExists(const std::string& path);
	std::string replaceDigits(const std::string& input);
	bool isSocketValid(int);
	std::string trimString(const std::string&);
	std::string toLowerCase(const std::string&);

	std::vector<char>::iterator find(std::vector<char>&, const std::string&);
	ssize_t findIndex(std::vector<char>&, const std::string&);
	ssize_t findIndex(std::vector<char>&, const std::string&, const ssize_t&);
	std::string toString(const std::vector<char>&);
}
