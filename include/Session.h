#pragma once

#include "webserv.h"

class Session {
public:
    static const int SESSION_LIFETIME = 1800; // 30 minutes
    
    Session();
    Session(const std::string& id);
    ~Session();

    std::string getId() const;
    void setValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key) const;
    bool isValid() const;
    void destroy();
    static std::string generateId();
    void refresh();
    time_t getLastAccess() const;
    void setUserData(const std::string& userId);

private:
    std::string _id;
    std::map<std::string, std::string> _data;
    time_t _lastAccess;
}; 