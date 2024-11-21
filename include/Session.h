#pragma once

#include "webserv.h"

class Session {
public:
    Session();
    Session(const std::string& id);
    ~Session();

    std::string getId() const;
    void setValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key) const;
    bool hasKey(const std::string& key) const;
    void remove(const std::string& key);
    bool isExpired() const;
    void refresh();

private:
    std::string _id;
    std::map<std::string, std::string> _data;
    time_t _lastAccess;
    static const int SESSION_TIMEOUT = 1800; // 30 minutes
};
