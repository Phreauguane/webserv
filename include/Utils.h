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
	std::string readFD(int);
	std::string toString(int);
	std::string toString(float);
	std::string toString(size_t);
	std::vector<std::string> splitString(const std::string&, const std::string&);
	int inet_pton_v4(const std::string&, in_addr_t*);
	void verify_args(const std::vector<std::string>&, size_t, size_t);
	template <typename T>
	bool searchFor(const std::vector<T>& vec, const T& e)
	{
		return std::find(vec.begin(), vec.end(), e) != vec.end();
	}
	bool compareAfterDot(const std::string &str, const char *cmp);
}
