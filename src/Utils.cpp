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
	timeStream	<< (localTime->tm_hour < 10 ? "0" : "") << localTime->tm_hour << "|"
				<< (localTime->tm_min < 10 ? "0" : "") << localTime->tm_min << "|"
				<< (localTime->tm_sec < 10 ? "0" : "") << localTime->tm_sec;
	
	// Return the formatted string
	return replaceDigits(timeStream.str());
}

std::string Utils::readFile(const std::string& filename, bool useEpoll)
{
    // Si c'est le fichier de configuration, pas besoin d'epoll
    if (!useEpoll) {
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
    // Pour les autres fichiers, utiliser epoll
    else {
        int fd = open(filename.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd < 0)
            throw std::runtime_error("Failed to open file : " + filename);
            
        // Ajouter le descripteur à epoll est complexe ici car nous n'avons pas accès à Config
        // Une solution serait de lire le fichier par petits morceaux et de vérifier EAGAIN
        
        std::string content;
        char buffer[4096];
        ssize_t bytesRead;
        
        while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
            content.append(buffer, bytesRead);
        }
        
        if (bytesRead < 0) {
            close(fd);
            throw std::runtime_error("Error reading file : " + filename);
        }
        
        close(fd);
        return content;
    }
}

std::string Utils::readFD(int fd, bool isEpollManaged) {
    if (!isEpollManaged) {
        throw std::runtime_error("Reading from FD without epoll management is not allowed");
    }
    
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

std::string Utils::toString(ssize_t s)
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

std::string Utils::generateRandomString(size_t length)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string result;
	result.reserve(length);

	srand(time(NULL));
	for (size_t i = 0; i < length; ++i)
		result += alphanum[rand() % (sizeof(alphanum) - 1)];
	return result;
}

bool Utils::fileExists(const std::string& path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::string Utils::replaceDigits(const std::string& input) {
    std::string result = input;
    
    const char* enclosedDigits[10] = {
        "🯰 ", // 🯰 (U+1BF0)
        "🯱 ", // 🯱 (U+1BF1)
        "🯲 ", // 🯲 (U+1BF2)
        "🯳 ", // 🯳 (U+1BF3)
        "🯴 ", // 🯴 (U+1BF4)
        "🯵 ", // 🯵 (U+1BF5)
        "🯶 ", // 🯶 (U+1BF6)
        "🯷 ", // 🯷 (U+1BF7)
        "🯸 ", // 🯸 (U+1BF8)
        "🯹 "  // 🯹 (U+1BF9)
    };
    
    size_t pos = 0;
    while ((pos = result.find_first_of("0123456789", pos)) != std::string::npos) {
        int digit = result[pos] - '0';
        result.replace(pos, 1, enclosedDigits[digit]);
        pos += 3;
    }
    
    return result;
}

bool Utils::isSocketValid(int socketFd) {
    // Vérifier si le descripteur existe
    if (fcntl(socketFd, F_GETFL) < 0) {
        return false;
    }
    
    // Vérifier l'état du socket
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0) {
        return false;
    }
    
    return true;
}

std::string Utils::trimString(const std::string& str) {
	std::string::size_type start = str.find_first_not_of(" \t");
	if (start == std::string::npos) {
		return "";
	}
	std::string::size_type end = str.find_last_not_of(" \t");
	return str.substr(start, end - start + 1);
}

std::string Utils::toLowerCase(const std::string& str) {
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i) {
		result[i] = tolower(result[i]);
	}
	return result;
}

std::vector<char>::iterator Utils::find(std::vector<char>& vec, const std::string& str) {
    if (str.empty() || vec.empty() || str.size() > vec.size()) {
        return vec.end();
    }
    
    return std::search(vec.begin(), vec.end(), str.begin(), str.end());
}

ssize_t Utils::findIndex(std::vector<char>& vec, const std::string& str) {
    if (str.empty() || vec.empty() || str.size() > vec.size())
        return -1;
    
    std::vector<char>::iterator res = std::search(vec.begin(), vec.end(), str.begin(), str.end());

	if (res == vec.end())
		return -1;
	return res - vec.begin();
}

ssize_t Utils::findIndex(std::vector<char>& vec, const std::string& str, const ssize_t& start) {
    if (str.empty() || vec.empty() || (str.size() + start) > vec.size())
        return -1;
    
    std::vector<char>::iterator res = std::search(vec.begin() + start, vec.end(), str.begin(), str.end());

	if (res == vec.end())
		return -1;
	return res - vec.begin();
}

std::string Utils::toString(const std::vector<char> &vec) {
	if (vec.size() == 0)
		return "";
	return std::string(vec.data());
}