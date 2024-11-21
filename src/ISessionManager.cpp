#include "ISessionManager.h"

ISessionManager::ISessionManager()
{
}

ISessionManager::ISessionManager(const ISessionManager& other)
{
	(void)other;
}

ISessionManager& ISessionManager::operator=(const ISessionManager& other)
{
	(void)other;
	return *this;
}

ISessionManager::~ISessionManager()
{
}