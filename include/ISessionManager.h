#pragma once
#include "Session.h"
#include <string>

class ISessionManager
{
public:
    ISessionManager();
    ISessionManager(const ISessionManager& other);
    ISessionManager& operator=(const ISessionManager& other);
    virtual ~ISessionManager();

    virtual bool hasSession(const std::string& sessionId) = 0;
    virtual Session* getSession(const std::string& sessionId) = 0;
};