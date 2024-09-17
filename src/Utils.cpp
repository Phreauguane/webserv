/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jde-meo <jde-meo@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 00:27:44 by jde-meo           #+#    #+#             */
/*   Updated: 2024/09/17 15:46:17 by jde-meo          ###   ########.fr       */
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
	if (bracket < source.size())
		output += '}';
	return output;
}

std::string Utils::removeWSpaces(const std::string& line)
{
	std::string output;
	size_t i = 0;
	while (i < line.size() && std::isspace(line[i]))
		++i;
	size_t j = line.size() - 1;
	while (j >= i && std::isspace(line[j]))
		--j;
	while (i <= j && i < line.size())
		output += line[i++];
	return output;
}

std::string Utils::removeSemicolon(const std::string& line)
{
	if (line.size() == 0)
		return line;
	size_t end = line.size() - 1;
	while (end > 0 && std::isspace(line[end]))
		--end;
	if (end > 0 && line[end] == ';')
		return line.substr(0, end);
	return line;
}

std::vector<std::string> Utils::splitString(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = str.find(delimiter);
    while (end != std::string::npos) {
        tokens.push_back(removeWSpaces(str.substr(start, end - start)));  // Trim each token
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(removeWSpaces(str.substr(start)));
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].size() == 0)
		{
			tokens.erase(tokens.begin() + i);
			--i;
		}
	}
	
    return tokens;
}