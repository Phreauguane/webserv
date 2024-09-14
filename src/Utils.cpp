/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 00:27:44 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/15 00:49:22 by jde-meo          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.h"


std::string Utils::removeComments(const std::string& source)
{
	std::string output("");
	for (size_t i = 0; i < source.size(); i++)
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

std::string Utils::readBrackets(const std::string& source, size_t start)
{
	/*
		Function that reads brackets from start to finish (recursively)
		Takes full source and start index as input

		will output a substring containing everything before the first '{'
		with the '{' and it's corresponding '}' and everything in between

		readBrackets("hey{{} miaou{ { miaou } { miaou 2 } }", 6); -> returns -> "miaou{ { miaou } { miaou 2 } }"
	*/

	size_t bracket = source.find('{', start);
	if (bracket == std::string::npos)
		return source.substr(start, source.size() - start);
	std::string output = source.substr(start, bracket - start) + '{';
	bracket++;
	while (source[bracket] != '}' && bracket < source.size())
	{
		if (source[bracket] == '{')
		{
			std::string substr = Utils::readBrackets(source, bracket);
			output += substr;
			bracket += substr.size();
			continue;
		}
		output += source[bracket];
		bracket++;
	}
	output += '}';
	return output;
}