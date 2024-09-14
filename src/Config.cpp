/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/11 23:10:32 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/14 13:41:26 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.h"

std::string Config::_readFile(const std::string& filename)
{
	std::string out = "";
	std::ifstream file(filename.c_str());
	while (file)
	{
		char c;
		file.read(&c, 1);

		if (file)
			out += c;
	}
	file.close();
	return out;
}

std::string Config::_removeComments(const std::string& source)
{
	std::string output("");
	for (int i = 0; i < source.size(); i++)
	{
		// ------ Doesn't seem to be usefull ------
		//
		// if (source[i] == '\'')
		// 	while (source[++i] != '\'' && i < source.size())
		// 		output += source[i];
		// if (source[i] == '\"')
		// 	while (source[++i] != '\"' && i < source.size())
		// 		output += source[i];
		//
		// ----------------------------------------
		if (source[i] == '#')
			while (source[i++] != '\n' && i < source.size());
		else
			output += source[i];
	}
	
	return output;
}

Config::Config() : Config("default.conf")
{}

Config::Config(const std::string& filename)
{
	_source = _readFile(filename);

}