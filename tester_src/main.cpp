#include "webserv.h"

// Forward declarations
std::string parseHttpResponse(const std::string& response);
std::string parseStatusLine(const std::string& statusLine);
std::string parseHeaders(const std::string& headers);
std::string parseBody(const std::string& body, const std::string& contentType);
bool startsWith(const std::string& str, const std::string& prefix);

// Parses and returns HTML with syntax highlighting
std::string parseHtml(const std::string& html) {
    std::string result = "";
    size_t pos = 0;
    bool inTag = false;
    bool inTagName = false;
    bool inAttribute = false;
    bool inAttributeValue = false;
    bool inComment = false;
    char quoteChar = '\0';

    while (pos < html.length()) {
        char c = html[pos];

        if (inComment) {
            if (pos + 2 < html.length() && html.substr(pos, 3) == "-->") {
                result += PURPLE "-->"; // Comment end
                inComment = false;
                pos += 3;
            } else {
                result += PURPLE;
                result += c;
                pos++;
            }
        } else if (inTag) {
            if (inAttributeValue) {
                if (c == quoteChar) {
                    result += YELLOW;
                    result += c;
                    inAttributeValue = false;
                } else {
                    result += YELLOW;
                    result += c;
                }
                pos++;
            } else if (inAttribute) {
                if (c == '=') {
                    result += CYAN;
                    result += c;
                    pos++;
                    if (pos < html.length() && (html[pos] == '"' || html[pos] == '\'')) {
                        quoteChar = html[pos];
                        result += YELLOW;
                        result += quoteChar;
                        inAttributeValue = true;
                        inAttribute = false;
                        pos++;
                    }
                } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                    result += c;
                    inAttribute = false;
                    pos++;
                } else {
                    result += CYAN;
                    result += c;
                    pos++;
                }
            } else if (inTagName) {
                if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                    result += c;
                    inTagName = false;
                    pos++;
                } else if (c == '>') {
                    result += BOLD_PURPLE;
                    result += c;
                    inTagName = false;
                    inTag = false;
                    pos++;
                } else {
                    result += BOLD_PURPLE;
                    result += c;
                    pos++;
                }
            } else if (c == '>') {
                result += BOLD_PURPLE;
                result += c;
                inTag = false;
                pos++;
            } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                result += c;
                pos++;
            } else if (c == '/') {
                result += BOLD_PURPLE;
                result += c;
                pos++;
            } else {
                result += CYAN;
                result += c;
                inAttribute = true;
                pos++;
            }
        } else if (c == '<') {
            if (pos + 3 < html.length() && html.substr(pos, 4) == "<!--") {
                result += PURPLE "<!--";
                inComment = true;
                pos += 4;
            } else {
                result += BOLD_PURPLE;
                result += c;
                inTag = true;
                inTagName = true;
                pos++;
            }
        } else {
            result += WHITE;
            result += c;
            pos++;
        }
    }

    return result;
}

// Parses and returns CSS with syntax highlighting
std::string parseCss(const std::string& css) {
    std::string result = "";
    size_t pos = 0;
    bool inSelector = true;
    bool inProperty = false;
    bool inValue = false;
    bool inComment = false;

    while (pos < css.length()) {
        char c = css[pos];

        if (inComment) {
            if (pos + 1 < css.length() && css[pos] == '*' && css[pos + 1] == '/') {
                result += PURPLE "*/";
                inComment = false;
                pos += 2;
            } else {
                result += PURPLE;
                result += c;
                pos++;
            }
        } else if (pos + 1 < css.length() && c == '/' && css[pos + 1] == '*') {
            result += PURPLE "/*";
            inComment = true;
            pos += 2;
        } else if (inSelector) {
            if (c == '{') {
                result += BOLD_CYAN;
                result += c;
                inSelector = false;
                inProperty = true;
                pos++;
            } else {
                result += YELLOW;
                result += c;
                pos++;
            }
        } else if (inProperty) {
            if (c == ':') {
                result += WHITE;
                result += c;
                inProperty = false;
                inValue = true;
                pos++;
            } else if (c == '}') {
                result += BOLD_CYAN;
                result += c;
                inProperty = false;
                inSelector = true;
                pos++;
            } else {
                result += BOLD_GREEN;
                result += c;
                pos++;
            }
        } else if (inValue) {
            if (c == ';') {
                result += WHITE;
                result += c;
                inValue = false;
                inProperty = true;
                pos++;
            } else if (c == '}') {
                result += BOLD_CYAN;
                result += c;
                inValue = false;
                inSelector = true;
                pos++;
            } else {
                result += CYAN;
                result += c;
                pos++;
            }
        } else {
            result += WHITE;
            result += c;
            pos++;
        }
    }

    return result;
}

// Parses and returns JavaScript with syntax highlighting
std::string parseJs(const std::string& js) {
    std::string result = "";
    size_t pos = 0;
    bool inString = false;
    bool inComment = false;
    bool inLineComment = false;
    char stringDelimiter = '\0';
    
    // Set of JavaScript keywords
    std::string keywords[] = {
        "function", "var", "let", "const", "if", "else", "for", "while", "do", 
        "switch", "case", "break", "continue", "return", "this", "new", "try", 
        "catch", "finally", "throw", "class", "import", "export", "default", "null", 
        "undefined", "true", "false"
    };
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);

    while (pos < js.length()) {
        char c = js[pos];

        if (inLineComment) {
            if (c == '\n') {
                result += PURPLE;
                result += c;
                inLineComment = false;
            } else {
                result += PURPLE;
                result += c;
            }
            pos++;
        } else if (inComment) {
            if (pos + 1 < js.length() && js[pos] == '*' && js[pos + 1] == '/') {
                result += PURPLE "*/";
                inComment = false;
                pos += 2;
            } else {
                result += PURPLE;
                result += c;
                pos++;
            }
        } else if (inString) {
            if (c == '\\' && pos + 1 < js.length()) {
                result += YELLOW;
                result += c;
                result += js[pos + 1];
                pos += 2;
            } else if (c == stringDelimiter) {
                result += YELLOW;
                result += c;
                inString = false;
                pos++;
            } else {
                result += YELLOW;
                result += c;
                pos++;
            }
        } else if (pos + 1 < js.length() && c == '/' && js[pos + 1] == '/') {
            result += PURPLE "//";
            inLineComment = true;
            pos += 2;
        } else if (pos + 1 < js.length() && c == '/' && js[pos + 1] == '*') {
            result += PURPLE "/*";
            inComment = true;
            pos += 2;
        } else if (c == '"' || c == '\'' || c == '`') {
            result += YELLOW;
            result += c;
            inString = true;
            stringDelimiter = c;
            pos++;
        } else if (isalpha(c) || c == '_' || (isalnum(c) && pos > 0 && (isalpha(js[pos-1]) || js[pos-1] == '_'))) {
            // Extract a word
            std::string word = "";
            while (pos < js.length() && (isalnum(js[pos]) || js[pos] == '_')) {
                word += js[pos];
                pos++;
            }
            
            // Check if it's a keyword
            bool isKeyword = false;
            for (int i = 0; i < numKeywords; i++) {
                if (word == keywords[i]) {
                    result += BOLD_BLUE + word;
                    isKeyword = true;
                    break;
                }
            }
            
            if (!isKeyword) {
                result += WHITE + word;
            }
        } else if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']') {
            result += BOLD_CYAN;
            result += c;
            pos++;
        } else if (c == ';' || c == ',' || c == '.') {
            result += BOLD_WHITE;
            result += c;
            pos++;
        } else {
            result += WHITE;
            result += c;
            pos++;
        }
    }

    return result;
}

std::string readResponse(int sockfd, size_t timeout) {
    std::string response = "";
    bool done = false;
    fd_set readfds;
    struct timeval tv;

    while (!done) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        
        int select_result = select(sockfd + 1, &readfds, NULL, NULL, &tv);
        
        if (select_result < 0) {
            std::cerr << BOLD_RED "Select error: " RED << strerror(errno) << DEF << std::endl;
            break;
        } else if (select_result == 0) {
            std::cout << BOLD_RED "Timeout waiting for server response." DEF << std::endl;
            break;
        } else {
            std::vector<char> buffer(2049);
            ssize_t bytes = recv(sockfd, buffer.data(), 2048, 0);
            
            if (bytes < 0) {
                std::cerr << BOLD_RED "Error receiving data: " RED << strerror(errno) << DEF << std::endl;
                break;
            } else if (bytes == 0) {
                std::cout << BOLD_GREEN "Server closed connection" << DEF << std::endl;
                done = true;
            } else {
                buffer[bytes] = '\0';
                response += buffer.data();
                
                if (bytes < 2048) {
                    usleep(100000);
                    
                    fd_set check_set;
                    FD_ZERO(&check_set);
                    FD_SET(sockfd, &check_set);
                    
                    struct timeval short_wait = {0, 0};
                    if (select(sockfd + 1, &check_set, NULL, NULL, &short_wait) <= 0) {
                        done = true;
                    }
                }
            }
        }
    }

    return response;
}

// Utility function to check if a string starts with a specific prefix
bool startsWith(const std::string& str, const std::string& prefix) {
    return str.substr(0, prefix.length()) == prefix;
}

// Parse HTTP status line
std::string parseStatusLine(const std::string& statusLine) {
    std::string result = "";
    size_t pos = 0;
    size_t end = statusLine.find("\r\n");
    
    if (end == std::string::npos) {
        end = statusLine.length();
    }
    
    // Extract HTTP version
    size_t httpPos = statusLine.find("HTTP/");
    if (httpPos != std::string::npos) {
        size_t versionEnd = statusLine.find(" ", httpPos);
        if (versionEnd != std::string::npos) {
            result += GREEN + statusLine.substr(httpPos, versionEnd - httpPos) + DEF + " ";
            pos = versionEnd + 1;
        }
    }
    
    // Extract status code
    size_t codeEnd = statusLine.find(" ", pos);
    if (codeEnd != std::string::npos) {
        result += BOLD_RED + statusLine.substr(pos, codeEnd - pos) + DEF + " ";
        pos = codeEnd + 1;
    }
    
    // Extract reason phrase
    if (pos < end) {
        result += YELLOW + statusLine.substr(pos, end - pos) + DEF;
    }
    
    if (end < statusLine.length()) {
        result += "\r\n";
    }
    
    return result;
}

// Parse HTTP headers
std::string parseHeaders(const std::string& headers) {
    std::string result = "";
    size_t pos = 0;
    size_t nextLine = headers.find("\r\n");
    
    while (nextLine != std::string::npos) {
        std::string line = headers.substr(pos, nextLine - pos);
        size_t colonPos = line.find(":");
        
        if (colonPos != std::string::npos) {
            // Header name
            result += BOLD_BLUE + line.substr(0, colonPos + 1) + DEF;
            
            // Header value
            if (colonPos + 1 < line.length()) {
                result += CYAN + line.substr(colonPos + 1) + DEF;
            }
        } else {
            // Not a valid header, just output as is
            result += line;
        }
        
        result += "\r\n";
        pos = nextLine + 2;
        nextLine = headers.find("\r\n", pos);
    }
    
    return result;
}

// Parse HTTP body based on content type
std::string parseBody(const std::string& body, const std::string& contentType) {
    if (startsWith(contentType, "text/html")) {
        return parseHtml(body);
    } else if (startsWith(contentType, "text/css")) {
        return parseCss(body);
    } else if (startsWith(contentType, "application/javascript") || 
               startsWith(contentType, "text/javascript")) {
        return parseJs(body);
    } else {
        // Default case: just return the body with white coloring
        return WHITE + body + DEF;
    }
}

// Main function to parse HTTP response
std::string parseHttpResponse(const std::string& response) {
    // size_t pos = 0;
    size_t headerEnd = response.find("\r\n\r\n");
    
    if (headerEnd == std::string::npos) {
        // No body found, just highlight what we can
        return parseStatusLine(response);
    }
    
    // Parse status line
    size_t statusEnd = response.find("\r\n");
    std::string statusLine = response.substr(0, statusEnd);
    std::string parsedStatus = parseStatusLine(statusLine);
    
    // Parse headers
    std::string headers = response.substr(statusEnd + 2, headerEnd - statusEnd - 2);
    std::string parsedHeaders = parseHeaders(headers);
    
    // Extract body
    std::string body = response.substr(headerEnd + 4);
    
    // Find content type
    std::string contentType = "text/plain"; // Default
    size_t contentTypePos = headers.find("Content-Type:");
    if (contentTypePos == std::string::npos) {
        contentTypePos = headers.find("content-type:");
    }
    
    if (contentTypePos != std::string::npos) {
        size_t valueStart = headers.find(":", contentTypePos) + 1;
        if (valueStart != std::string::npos) {
            while (valueStart < headers.length() && 
                  (headers[valueStart] == ' ' || headers[valueStart] == '\t')) {
                valueStart++;
            }
            
            size_t valueEnd = headers.find("\r\n", valueStart);
            if (valueEnd != std::string::npos) {
                contentType = headers.substr(valueStart, valueEnd - valueStart);
                size_t semicolonPos = contentType.find(";");
                if (semicolonPos != std::string::npos) {
                    contentType = contentType.substr(0, semicolonPos);
                }
            }
        }
    }
    
    // Parse body based on content type
    std::string parsedBody = parseBody(body, contentType);
    
    return parsedStatus + "\r\n" + parsedHeaders + "\r\n" + parsedBody;
}

std::string length(const std::string &str, size_t length) {
    std::string string = str;
    while (string.length() < length) {
        string += " ";
    }
    return string;
}

int main(int argc, char* argv[]) {
    if (argc != 3 && argc != 4) {
        std::cerr << BOLD_RED "Usage: " RED << argv[0] << " <server_ip> <port> (timeout)" DEF << std::endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);
    size_t timeout = (argc == 4) ? atoi(argv[3]) : 5000;
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    {
        if (sockfd < 0) {
            std::cerr << BOLD_RED "Error creating socket: " << strerror(errno) << DEF << std::endl;
            return 1;
        }

        struct sockaddr_in server_addr;
        std::memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        
        if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
            std::cerr << BOLD_RED "Invalid address: " DEF << strerror(errno) << std::endl;
            close(sockfd);
            return 1;
        }
        
        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            std::cerr << BOLD_RED "Connection failed: " DEF << strerror(errno) << std::endl;
            close(sockfd);
            return 1;
        }
    }

    std::stringstream ss;
    
    std::cout << BOLD_RED "╭───────────────────────────────────────────────╮" << DEF << std::endl;
    std::cout << BOLD_RED "│ ╭─────────────── HTTP TESTER ───────────────╮ │" << DEF << std::endl;
    std::cout << BOLD_RED "│ │ "; ss << "Connected to " << server_ip << ":" << port; std::cout << BOLD_GREEN << length(ss.str(), 41) << BOLD_RED " │ │" << std::endl; 
    std::cout << BOLD_RED "│ ╰───────────────────────────────────────────╯ │" << DEF << std::endl;
    std::cout << BOLD_RED "│       " GREEN "▷           Commands           ◁" BOLD_RED "        │" << DEF << std::endl;
    std::cout << BOLD_RED "│       " GREEN "▏ " YELLOW "exit: " WHITE "quits" GREEN "                  ▕" BOLD_RED "        │" << DEF << std::endl;
    std::cout << BOLD_RED "│       " GREEN "▏ " YELLOW "send: " WHITE "sends request to server" GREEN "▕" BOLD_RED "        │" << DEF << std::endl;
    std::cout << BOLD_RED "│       " GREEN "▷      Type your request       ◁" BOLD_RED "        │" << DEF << std::endl;
    std::cout << BOLD_RED "│                                               │" << DEF << std::endl;
    std::cout << BOLD_RED "╰───────────────────────────────────────────────╯" << DEF << std::endl;
    std::cout << "" << DEF << std::endl;
    
    char buffer[1024];
    std::string request = "";
    
    while (true) {
        std::cout << PURPLE "> " CYAN;
        std::string message;
        std::getline(std::cin, message);
        
        if (message == "exit") {
            break;
        } else if (message == "send") {
            // request += "\r\n";
            std::cout << BOLD_GREEN "sending request..." DEF << std::endl;
            if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
                std::cerr << BOLD_RED "Failed to send data: " RED << strerror(errno) << DEF << std::endl;
                break;
            }
            
            std::memset(buffer, 0, sizeof(buffer));
            
            std::string response = readResponse(sockfd, timeout);
            std::cout << CYAN "Received: \n" DEF;
            
            // Parse and colorize the HTTP response
            std::string colorizedResponse = parseHttpResponse(response);
            std::cout << colorizedResponse << std::endl;
            std::cout << DEF; // Reset terminal colors
            
            request = "";
        } else if (message == "reconnect") {
            close(sockfd);
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            {
                if (sockfd < 0) {
                    std::cerr << BOLD_RED "Error creating socket: " << strerror(errno) << DEF << std::endl;
                    return 1;
                }

                struct sockaddr_in server_addr;
                std::memset(&server_addr, 0, sizeof(server_addr));
                server_addr.sin_family = AF_INET;
                server_addr.sin_port = htons(port);
                
                if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
                    std::cerr << BOLD_RED "Invalid address: " DEF << strerror(errno) << std::endl;
                    close(sockfd);
                    return 1;
                }
                
                if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                    std::cerr << BOLD_RED "Connection failed: " DEF << strerror(errno) << std::endl;
                    close(sockfd);
                    return 1;
                }
            }
        } else {
            request += message + "\r\n";
        }
        
    }
    
    close(sockfd);
    return 0;
}