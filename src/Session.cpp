#include "Session.h"

Session::Session() : _lastAccess(time(NULL)) {
    // Générer un ID de session unique
    std::stringstream ss;
    ss << std::hex << time(NULL) << rand();
    _id = ss.str();
}

Session::Session(const std::string& id) : _id(id), _lastAccess(time(NULL)) {}

Session::~Session() {}

std::string Session::getId() const {
    return _id;
}

void Session::setValue(const std::string& key, const std::string& value) {
    _data[key] = value;
    refresh();
}

std::string Session::getValue(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = _data.find(key);
    return (it != _data.end()) ? it->second : "";
}

bool Session::hasKey(const std::string& key) const {
    return _data.find(key) != _data.end();
}

void Session::remove(const std::string& key) {
    _data.erase(key);
    refresh();
}

bool Session::isExpired() const {
    return (time(NULL) - _lastAccess) > SESSION_TIMEOUT;
}

void Session::refresh() {
    _lastAccess = time(NULL);
}
