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
		{
			while (source[i++] != '\n' && i < source.size());
			output += "\n";
		}
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
	if (line.size() == 0)
		return line;
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
		tokens.push_back(removeWSpaces(str.substr(start, end - start)));
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

int Utils::inet_pton_v4(const std::string& ip_str, in_addr_t *addr) {
	// Split the IP string by '.' and store in an array
	unsigned int octets[4];
	std::istringstream iss(ip_str);
	char dot;

	// Parse the string using dots as delimiters
	if ((iss >> octets[0] >> dot >> octets[1] >> dot >> octets[2] >> dot >> octets[3]) &&
		dot == '.' && (octets[0] <= 255 && octets[1] <= 255 && octets[2] <= 255 && octets[3] <= 255)) {
		// Convert the IP string to a 32-bit integer in network byte order (big-endian)
		*(addr) = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
		return 1; // Success
	}

	return 0; // Error: invalid format
}

void Utils::verify_args(const std::vector<std::string>& strs, size_t min, size_t max)
{
	if (strs.size() >= min && strs.size() <= max)
		return;
	std::string line = strs[0];
	for (size_t i = 1; i < strs.size(); i++)
	{
		line += " " + strs[i];
	}

	throw std::runtime_error("Invalid argument list : " + line);
}

std::string Utils::getCurrentTime()
{
	// Get current time as a time_t object
	std::time_t currentTime = std::time(NULL);
	
	// Convert it to local time
	std::tm* localTime = std::localtime(&currentTime);
	
	// Create a string stream to format the time
	std::ostringstream timeStream;
	
	// Output the time in "Hours:Minutes:Seconds" format
	timeStream	<< (localTime->tm_hour < 10 ? "0" : "") << localTime->tm_hour << ":"
				<< (localTime->tm_min < 10 ? "0" : "") << localTime->tm_min << ":"
				<< (localTime->tm_sec < 10 ? "0" : "") << localTime->tm_sec;
	
	// Return the formatted string
	return timeStream.str();
}

std::string Utils::readFile(const std::string& filename)
{
	std::string out = "";
	std::ifstream file(filename.c_str());

	if (!file.is_open())
		throw std::runtime_error("Failed to open file : " + filename);
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

std::string Utils::readFD(int fd) {
	char buffer[1024];
	std::string result;
	ssize_t bytesRead;

	while ((bytesRead = read(fd, buffer, 1024)) > 0) {
		result.append(buffer, bytesRead);
	}

	if (bytesRead == -1) {
		throw std::runtime_error("read error in Utils::readFD");
	}

	return result;
}

std::string Utils::toString(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

std::string Utils::toString(float f)
{
	std::stringstream ss;
	ss << f;
	return ss.str();
}

std::string Utils::toString(size_t s)
{
	std::stringstream ss;
	ss << s;
	return ss.str();
}

std::string Utils::getExt(const std::string &str)
{
	size_t pos = str.find_last_of(".");
	size_t pos_2 = str.find("?");
	
	if (pos == std::string::npos || pos > pos_2)
		return "";
	
	size_t size = str.size() - pos;
	if (pos_2 == std::string::npos)
		size = pos_2 - pos;
	
	return str.substr(pos, size);
}

bool Utils::uploadFiles(std::string files, std::string path, std::string fileName)
{
	// Construire le chemin complet du fichier
	std::string fullPath = path + "/upload/" + fileName;
	// Créer et ouvrir le fichier dans le dossier spécifié
	std::ofstream file;

	file.open(fullPath.c_str(), std::ios::out);
	if (!file)
	{
		Logger::log("FAIL UPLOAD on location : " + fullPath, DEBUG);
		return false;
	}
	else
	{
		file << files;
		Logger::log("UPLOAD on location : " + fullPath, DEBUG);
	}
	file.close(); // Fermer le fichier
	return 0;
}
