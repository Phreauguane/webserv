/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 15:56:35 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/30 12:04:28 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.h"

Response::Response() {}

Response::Response(const Response &copy)
{
	*this = copy;
}

Response &Response::operator=(const Response& copy)
{
	if (this == &copy)
		return *this;
	http = copy.http;
	status = copy.status;
	phrase = copy.phrase;
	attributes = copy.attributes;
	body = copy.body;
	ready = copy.ready;
	return *this;
}

std::string Response::build_header()
{

	std::stringstream header;
	header << http << " " << status << " " << phrase << "\r\n";
	
	// iterate through attributes
	for (std::map<std::string, std::string>::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
	{
		header << it->first << ": " << it->second << "\r\n";
	}
	
	header << "Content-Length: " << body.size() << "\r\n";
	header << "\r\n";  // Blank line between headers and body
	
	return header.str();
}

std::string Response::build()
{
	if (!ready)
		return "Not ready :)";

	return build_header() + body;
}

Response::~Response()
{
	//
}