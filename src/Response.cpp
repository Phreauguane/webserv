/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 15:56:35 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/27 16:03:41 by jde-meo          ###   ########.fr       */
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
	http = copy.http;
	status = copy.status;
	phrase = copy.phrase;
	content_type = copy.content_type;
	body = copy.body;
}

std::string Response::build()
{
	std::stringstream header;
	header << http << " " << status << " " << phrase << "\r\n";
	header << "Content-Length: " << body.size() << "\r\n";
	header << "Content-Type: " << content_type << "\r\n";
	header << "\r\n";  // Blank line between headers and body

	return header.str() + body;
}