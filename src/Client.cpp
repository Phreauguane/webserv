/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 16:20:16 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/27 15:52:05 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.h"

Client::Client() : _req(NULL) {}

Client::Client(const Client& copy) : _req(NULL) { *this = copy; }

Client::Client(Server *server) : _req(NULL)
{
	_server = server;
	_fd = accept(_server->getSockFd(), NULL, NULL);
	if (_fd < 0)
		throw std::runtime_error("Failed to accept connection to " + _server->getIp());
	_size = _server->getMaxBodySize();
	Logger::log("Client connected to " + _server->getIp(), SUCCESS);
}

Client::~Client()
{
	if (_req)
		delete _req;
	close(_fd);
}

Client& Client::operator=(const Client& copy)
{
	if (this == &copy)
		return *this;
	_server = copy._server;
	_request = copy._request;
	_fd = copy._fd;
	return *this;
}

int Client::getFd()
{
	return _fd;
}

bool Client::readRequest()
{
	char buffer[_size];
	std::memset(buffer, 0, _size);

	int rd = read(_fd, buffer, _size);
	if (rd < 0)
		throw std::runtime_error("Failed to read request to server " + _server->getIp());
	if (rd == 0)
		return false;
	_request.clear();
	_request = buffer;
	if (_req)
		delete _req;
	_req = new Request(_request);
	_response = _server->buildResponse(*_req);
	return true;
}

bool Client::sendMsg(const std::string& msg)
{
	const char *message = msg.c_str();
	ssize_t total_len = msg.size();
	ssize_t sent = send(_fd, message, total_len, MSG_NOSIGNAL);
	return sent == total_len;
}

std::string Client::getRequest()
{
	return _request;
}