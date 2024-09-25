/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 16:20:16 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/25 18:13:18 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.h"

Client::Client() {}

Client::Client(const Client& copy) { *this = copy; }

Client::Client(Server *server)
{
	_server = server;
	_fd = accept(_server->getSockFd(), NULL, NULL);
	if (_fd < 0)
		throw std::runtime_error("Failed to accept connection to " + _server->getIp());
	_size = _server->getMaxBodySize();
	Logger::log("Client connected to " + _server->getIp(), SUCCESS);
	// readRequest();
	// std::cout << "IN CONSTRUCTOR\n" << _request << std::endl;
}

Client::~Client()
{
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

/*
	reads from the client FD

	returns true if there is something to read
	returns false if the FD is empty
*/
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
	return true;
}

bool Client::sendMsg(const std::string& msg)
{
	std::string response = "HTTP/1.1 200 OK\r\n";
	std::stringstream length;
	length << "Content-Length: " << msg.length();
	response += length.str() + "\r\n";
	response += "Content-Type: text/plain\r\n";
	response += "\r\n";  // Blank line between headers and body
	response += msg;
	const char *message = response.c_str();
	ssize_t total_len = response.size();
	ssize_t sent = send(_fd, message, total_len, MSG_NOSIGNAL);
	return sent == total_len;
}

std::string Client::getRequest()
{
	return _request;
}