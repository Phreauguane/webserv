#pragma once

#include <string>

struct Cookie {
    std::string name;
    std::string value;
    std::string expires;
    std::string path;
    std::string domain;
    bool secure;
    bool httpOnly;
}; 