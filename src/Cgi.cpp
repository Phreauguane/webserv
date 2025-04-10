#include "Utils.h"
#include "Cgi.h"
#include "Server.h"

CGI::CGI() : _stdout(dup(STDOUT_FILENO)), _server(NULL) {}

CGI::CGI(Server *server) : _stdout(dup(STDOUT_FILENO)), _server(server) {}

CGI::CGI(const CGI& copy)
{
	*this = copy;
}

void CGI::setup(char **env)
{
	_env = env;
}

CGI& CGI::operator=(const CGI& copy)
{
	_stdout = copy._stdout;
	_env = copy._env;
	return *this;
}

CGI::~CGI()
{

}

void CGI::_restoreStdOut()
{
	dup2(_stdout, STDOUT_FILENO);
}

static int *createPipes()
{
	static int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("Failed to create pipe");
	return pipefd;
}

void CGI::_executeCommand(const std::string& cmd, const std::vector<char>& query, char **args, char **envp, int *pipefd)
{
	try {
		if (!cmd.c_str() || !args || !envp || !pipefd) {
			throw std::runtime_error("Invalid arguments passed to _executeCommand");
		}

		if (access(cmd.c_str(), F_OK | X_OK) != 0) {
			throw std::runtime_error("Command not found or not executable: " + cmd);
		}

		if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
			throw std::runtime_error("Failed to redirect stdout");
		}

		if (dup2(pipefd[1], STDERR_FILENO) == -1) {
			throw std::runtime_error("Failed to redirect stderr");
		}

		int *pipe_stdin = createPipes();
		if (!pipe_stdin) {
			throw std::runtime_error("Failed to create stdin pipe");
		}

		if (dup2(pipe_stdin[0], STDIN_FILENO) == -1) {
			close(pipe_stdin[0]);
			close(pipe_stdin[1]);
			throw std::runtime_error("Failed to redirect stdin");
		}

		close(pipe_stdin[0]);
		
		if (!query.empty()) {
			ssize_t written = write(pipe_stdin[1], query.data(), query.size());
			if (written == -1 || static_cast<size_t>(written) != query.size()) {
				close(pipe_stdin[1]);
				throw std::runtime_error("Failed to write query data");
			}
		}
		
		close(pipe_stdin[1]);
		close(pipefd[0]);
		close(pipefd[1]);

		if (execve(cmd.c_str(), args, envp) == -1) {
			_restoreStdOut();
			throw std::runtime_error("Failed to execute command");
		}
	}
	catch (const std::exception& e) {
		_restoreStdOut();
		Logger::log("CGI execution error: " + std::string(e.what()), ERROR);
		throw;
	}
}

std::string extractAfterPrefix(const std::string& input, const std::string& prefix)
{
	size_t pos = input.find(prefix);
		
	if (pos == std::string::npos)
		return "";

	pos += prefix.length();

	size_t endPos = input.find("\n", pos);

	if (endPos == std::string::npos)
		endPos = input.length();

	std::string result = input.substr(pos, endPos - pos);
	return result;
}

std::string removeHeader(const std::string& input)
{
	size_t pos = 0;
	size_t start = 0;
	bool foundContentType = false;
	std::string result;

	while ((pos = input.find('\n', start)) != std::string::npos)
	{
		std::string line = input.substr(start, pos - start);
		start = pos + 1;

		if (foundContentType)
			result += line + "\n";
		else if (line.find("Content-type: ") != std::string::npos)
			foundContentType = true;
	}

	std::string lastLine = input.substr(start);
	if (foundContentType)
		result += lastLine + "\n";

	return result;
}

Response CGI::_parseOutputPHP(std::string& output, Request& req)
{
	Response rep;
	rep.http = "HTTP/1.1";

	try {
		if (output.empty()) {
			throw std::runtime_error("Empty CGI output");
		}

		std::string contentType = Utils::removeWSpaces(extractAfterPrefix(output, "Content-type: "));
		if (!contentType.empty()) {
			rep.attributes["Content-Type"] = contentType;
		} else {
			rep.attributes["Content-Type"] = "text/html";
		}

		std::string status = Utils::removeWSpaces(extractAfterPrefix(output, "Status: "));
		if (!status.empty()) {
			rep.status = std::atoi(status.c_str());
			if (rep.status < 100 || rep.status > 599) {
				rep.status = 500;
			}
		} else {
			rep.status = 200;
		}

		std::string location = Utils::removeWSpaces(extractAfterPrefix(output, "Location: "));
		if (!location.empty()) {
			rep.attributes["Location"] = location;
			if (rep.status == 200) {
				rep.status = 302;
			}
		}

		rep.phrase = "OK";
		rep.body = removeHeader(output);
		rep.ready = true;

		_handlePHPSession(req, rep);
	}
	catch (const std::exception& e) {
		Logger::log("CGI output parsing error: " + std::string(e.what()), ERROR);
		rep.status = 500;
		rep.phrase = "Internal Server Error";
		rep.body = "Failed to process CGI output";
		rep.ready = true;
	}

	return rep;
}

Response CGI::_handleParentProcessToutCourt(int* pipefd, pid_t pid, Request& req)
{
    close(pipefd[1]);

	const size_t start = time(NULL) * 1000;

    std::string result;
    int status;

    while (true) {
        pid_t waitPid = waitpid(pid, &status, WNOHANG);

        if (waitPid == -1) {
            throw std::runtime_error("Error occurred while waiting for child process");
        }

        if (waitPid == pid) {
            if (WIFEXITED(status)) {
                Logger::log("Child process code: " + Utils::toString(WEXITSTATUS(status)), DEBUG);
            } else if (WIFSIGNALED(status)) {
                Logger::log("Child process was terminated by signal: " + Utils::toString(WTERMSIG(status)), DEBUG);
            }
            break;
        }

		size_t now = time(NULL) * 1000;
        if ((now - start) > _server->getCgiTimeout()) {
            kill(pid, SIGINT);
            throw std::runtime_error("Timeout");
        }

        usleep(100000);
    }

    try {
        result = Utils::readFD(pipefd[0], true);
        close(pipefd[0]);
    } catch (const std::exception& e) {
        close(pipefd[0]);
        throw std::runtime_error("Error reading from pipe: " + std::string(e.what()));
    }

    return _parseOutputPHP(result, req);
}

std::vector<char> CGI::_getQuery(const Request& req)
{
	if (req.method == "POST") {
		std::map<std::string, std::string>::const_iterator it = req.attributes.find("Content-Type");
		if (it == req.attributes.end()) {
			Logger::log("POST request missing Content-Type", ERROR);
			throw std::runtime_error("Missing Content-Type in POST request");
		}

		if (it->second.find("multipart/form-data") != std::string::npos) {
			try {
				std::string contentType = it->second;
				size_t boundaryPos = contentType.find("boundary=");
				if (boundaryPos == std::string::npos) {
					throw std::runtime_error("Missing boundary in multipart/form-data");
				}
				std::string boundary = contentType.substr(boundaryPos + 9);
				
				if (boundary.empty()) {
					throw std::runtime_error("Empty boundary in multipart/form-data");
				}

				std::string filename = "unknown";
				std::vector<char> request = req.request;
				ssize_t filenamePos = Utils::findIndex(request, std::string("filename=\""));
				if (filenamePos != -1) {
					filenamePos += 10;
					ssize_t filenameEnd = Utils::findIndex(request, "\"", filenamePos);
					if (filenameEnd == -1) {
						throw std::runtime_error("Malformed filename in multipart/form-data");
					}
					filename = std::string(request.begin() + filenamePos, request.begin() + filenameEnd);
				}

				if (req.body.empty()) {
					throw std::runtime_error("Empty body in POST request");
				}
				
				std::string header = "--" + boundary + "\r\n";
				header += "Content-Disposition: form-data; name=\"fichier\"; filename=\"" + filename + "\"\r\n";
				header += "Content-Type: " + contentType + "\r\n\r\n";
				std::string footer = "\r\n--" + boundary + "--\r\n";
				
				size_t totalSize = header.size() + req.body.size() + footer.size();
				if (totalSize > MAX_POST_SIZE) {
					throw std::runtime_error("POST request too large");
				}
				
				std::vector<char> body;
				try {
					body.reserve(totalSize);
					body.insert(body.end(), header.begin(), header.end());
					body.insert(body.end(), req.body.begin(), req.body.end());
					body.insert(body.end(), footer.begin(), footer.end());
				} catch (const std::bad_alloc& e) {
					throw std::runtime_error("Memory allocation failed for POST data");
				}
				
				return body;
			}
			catch (const std::exception& e) {
				Logger::log("Error in _getQuery: " + std::string(e.what()), ERROR);
				throw;
			}
		}
		return req.body;
	}
	
	if (req.method == "GET") {
		size_t pos = req.path.find("?");
		if (pos != std::string::npos) {
			return std::vector<char>(req.path.begin() + pos + 1, req.path.end());
		}
	}
	return std::vector<char>();
}

static std::string get_paths(char **envp)
{
	for (size_t i = 0; envp[i] != NULL; i++)
	{
		if (envp[i][0] == 'P' && envp[i][1] == 'A'
			&& envp[i][2] == 'T' && envp[i][3] == 'H'
			&& envp[i][4] == '=')
			return &(envp[i][5]);
	}
	return "";
}

std::string CGI::_getExec(std::string cmd)
{
	std::vector<std::string> paths;
	std::string paths_no_split;

	paths_no_split = get_paths(_env);
	if (paths_no_split.size() == 0)
		return cmd;
	paths = Utils::splitString(paths_no_split, ":");
	for (size_t i = 0; i < paths.size(); ++i)
	{
		paths[i] += "/" + cmd;
		if (access(paths[i].c_str(), F_OK | X_OK) == 0)
			return paths[i];
	}
	return cmd;
}

Response CGI::_execPHP(const std::string& path, Location *loc, Request& req)
{
	std::string php_cgi = _getExec("php-cgi");
	char *args[] = {(char*)php_cgi.c_str(), (char*)path.c_str(), NULL};

	std::string clean_path = path;
	size_t queryPos = clean_path.find("?");
	if (queryPos != std::string::npos) {
		clean_path = clean_path.substr(0, queryPos);
	}

	std::string session_id = "";
	Session* session = NULL;
	
	if (req.attributes.find("Cookie") != req.attributes.end())
	{
		std::string cookies = req.attributes["Cookie"];
		size_t pos = cookies.find("PHPSESSID=");
		if (pos != std::string::npos)
		{
			pos += 10;
			size_t end = cookies.find(";", pos);
			session_id = cookies.substr(pos, end - pos);
			if (_server && _server->hasSession(session_id))
				session = _server->getSession(session_id);
		}
	}

	std::vector<char> query = _getQuery(req);
	
	std::string request_method = "REQUEST_METHOD=" + req.method;
	std::string content_type = "CONTENT_TYPE=" + req.attributes["Content-Type"];
	std::string content_length = "CONTENT_LENGTH=" + Utils::toString(query.size());
	std::string script_filename = "SCRIPT_FILENAME=" + clean_path;
	std::string session_env = "HTTP_COOKIE=PHPSESSID=" + session_id;
	std::string upload_tmp_dir = "UPLOAD_DIR=" + (loc->_upload == "" ? "/uploads/" : loc->_upload);
	std::string gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
	std::string server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
	std::string document_root = "DOCUMENT_ROOT=" + clean_path.substr(0, clean_path.find_last_of("/"));
	std::string script_name = "SCRIPT_NAME=" + clean_path.substr(clean_path.find_last_of("/"));
	std::string query_string = "QUERY_STRING=";
	query_string.append(query.begin(), query.end());

	std::string session_data = "";
	if (session && session->hasKey("user_id"))
		session_data = "PHP_SESSION_VARS={\"user_id\":\"" + session->getValue("user_id") + "\"}";

	char* envp[] = {
		(char*)request_method.c_str(),
		(char*)content_length.c_str(),
		(char*)content_type.c_str(),
		(char*)script_filename.c_str(),
		(char*)document_root.c_str(),
		(char*)script_name.c_str(),
		(char*)"REDIRECT_STATUS=200",
		(char*)session_env.c_str(),
		(char*)upload_tmp_dir.c_str(),
		(char*)gateway_interface.c_str(),
		(char*)server_protocol.c_str(),
		(char*)query_string.c_str(),
		session_data.empty() ? NULL : (char*)session_data.c_str(),
		NULL
	};

	int* pipefd = createPipes();
	if (pipefd == NULL)
		throw std::runtime_error("Unable to create pipe");

	pid_t pid = fork();
	if (pid < 0)
		throw std::runtime_error("Unable to fork");
	else if (pid == 0)
	{
		try
		{
			_executeCommand(php_cgi, query, args, envp, pipefd);
		}
		catch(const std::runtime_error& e)
		{
			Logger::log("CGI execution error", ERROR);
			exit(EXIT_FAILURE);
		}
	}
	else {
		try {
			return _handleParentProcessToutCourt(pipefd, pid, req);
		} catch (const std::exception &e) {
			std::string str = e.what();
			if (str == "Timeout") {
				Logger::log("Timeout", INFO);
				return _server->errorPage(408);
			}
			Logger::log("CGI Error: " + str, ERROR);
			return _server->errorPage(500);
		}
	}
	Response rep;
	return rep;
}

Response CGI::_execPython(const std::string& path, Request& req)
{
	std::string python_version = _getPythonVersion();
	if (python_version >= "3.13") {
		Logger::log("Warning: Python CGI module is deprecated in Python 3.13+. Consider updating your Python scripts.", WARNING);
	}

	std::string clean_path = path;
	size_t pos = clean_path.find("?");
	if (pos != std::string::npos) {
		clean_path = clean_path.substr(0, pos);
	}

	std::string python_exec = _getExec("python3");
	char *args[] = {(char*)python_exec.c_str(), (char*)clean_path.c_str(), NULL};
	
	std::vector<char> query = _getQuery(req);
	std::string request_method = "REQUEST_METHOD=" + req.method;
	std::string content_type = "CONTENT_TYPE=" + req.attributes["Content-Type"];
	std::string content_length = "CONTENT_LENGTH=" + Utils::toString(query.size());
	std::string script_filename = "SCRIPT_FILENAME=" + clean_path;
	std::string gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
	std::string server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
	std::string query_string = "QUERY_STRING=";
	query_string.append(query.begin(), query.end());

	char* envp[] = {
		(char*)request_method.c_str(),
		(char*)content_length.c_str(),
		(char*)content_type.c_str(),
		(char*)script_filename.c_str(),
		(char*)gateway_interface.c_str(),
		(char*)server_protocol.c_str(),
		(char*)query_string.c_str(),
		NULL
	};

	int* pipefd = createPipes();
	if (pipefd == NULL)
		throw std::runtime_error("Unable to create pipe");

	pid_t pid = fork();
	if (pid < 0)
		throw std::runtime_error("Unable to fork");
	else if (pid == 0)
	{
		try {
			_executeCommand(python_exec, query, args, envp, pipefd);
		}
		catch(const std::runtime_error& e)
		{
			Logger::log("CGI execution error", ERROR);
			exit(EXIT_FAILURE);
		}
	}
	else {
		try {
			return _handleParentProcessToutCourt(pipefd, pid, req);
		} catch (const std::exception &e) {
			std::string str = e.what();
			if (str == "Timeout") {
				Logger::log("Timeout", INFO);
				return _server->errorPage(408);
			}
			Logger::log("CGI Error: " + str, ERROR);
			return _server->errorPage(500);
		}
	}
	Response rep;
	return rep;
}

std::string CGI::_getPythonVersion()
{
	std::string python_exec = _getExec("python3");
	std::string cmd = python_exec + " --version";
	
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe) {
		return "unknown";
	}
	
	char buffer[128];
	std::string result = "";
	
	while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
		result += buffer;
	}
	
	pclose(pipe);
	
	size_t pos = result.find(" ");
	if (pos != std::string::npos) {
		return result.substr(pos + 1);
	}
	
	return "unknown";
}

std::string CGI::_compileCProgram(const std::string& path)
{
	std::string output = path + ".out";
	std::string gcc = _getExec("gcc");
	
	std::string compile_cmd = gcc + " " + path + " -o " + output;
	if (system(compile_cmd.c_str()) != 0)
		throw std::runtime_error("Failed to compile C program");
	
	return output;
}

Response CGI::_execC(const std::string& path, Request& req)
{
	std::string clean_path = path;
	size_t pos = clean_path.find("?");
	if (pos != std::string::npos) {
		clean_path = clean_path.substr(0, pos);
	}
	
	std::vector<char> query = _getQuery(req);
	
	std::string executable = _compileCProgram(clean_path);
	
	int* pipefd = createPipes();
	if (pipefd == NULL)
		throw std::runtime_error("Unable to create pipe");

	pid_t pid = fork();
	if (pid < 0)
		throw std::runtime_error("Unable to fork");
	else if (pid == 0)
	{
		try
		{
			char* args[] = {(char*)executable.c_str(), NULL};
			
			std::string query_string = "QUERY_STRING=";
			query_string.append(query.begin(), query.end());
			char* envp[] = {
				(char*)query_string.c_str(),
				NULL
			};
			
			_executeCommand(executable, query, args, envp, pipefd);
		}
		catch(const std::runtime_error& e)
		{
			Logger::log("CGI execution error", ERROR);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		Response rep;

		try {
			rep = _handleParentProcessToutCourt(pipefd, pid, req);
			remove(executable.c_str());
			return rep;
		} catch (const std::exception &e) {
			std::string str = e.what();
			if (str == "Timeout") {
				Logger::log("Timeout", INFO);
				return _server->errorPage(408);
			}
			Logger::log("CGI Error: " + str, ERROR);
			return _server->errorPage(500);
		}
	}
	Response rep;
	return rep;
}

bool CGI::executeCGI(Request &req, const std::string &path, Location *loc, Response &rep)
{
	bool allowed = false;
	std::string clean_path = req.path;
	size_t pos = clean_path.find("?");
	if (pos != std::string::npos) {
		clean_path = clean_path.substr(0, pos);
	}

	std::string ext = "";
	size_t dot_pos = clean_path.find_last_of(".");
	if (dot_pos != std::string::npos) {
		ext = clean_path.substr(dot_pos);
	}
	
	if (ext.empty())
		return false;

	for (size_t i = 0; i < loc->_cgi_exts.size(); ++i)
	{
		if (ext == loc->_cgi_exts[i])
		{
			allowed = true;
			break;
		}
	}
	if (!allowed)
		return false;

	if (ext == ".php")
	{
		Logger::log("Running php...", DEBUG);
		rep = _execPHP(path, loc, req);
	}
	else if (ext == ".py")
	{
		Logger::log("Running python...", DEBUG);
		rep = _execPython(path, req);
	}
	else if (ext == ".c")
	{
		Logger::log("Running C program...", DEBUG);
		rep = _execC(path, req);
	}
	else
	{
		Logger::log("Unknown ext : " + ext, DEBUG);
		return false;
	}
	return true;
}

void CGI::_handlePHPSession(Request& req, Response& rep)
{
	std::string session_id = "";
	if (req.attributes.find("Cookie") != req.attributes.end())
	{
		std::string cookies = req.attributes["Cookie"];
		size_t pos = cookies.find("PHPSESSID=");
		if (pos != std::string::npos)
		{
			pos += 10;
			size_t end = cookies.find(";", pos);
			session_id = cookies.substr(pos, end - pos);
		}
	}

	if (session_id.empty())
	{
		session_id = Utils::generateRandomString(32);
		rep.attributes["Set-Cookie"] = "PHPSESSID=" + session_id + "; Path=/";
	}
}