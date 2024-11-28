#include "Utils.h"
#include "Cgi.h"

CGI::CGI() : _stdout(dup(STDOUT_FILENO)), _server(NULL) {}

CGI::CGI(ISessionManager& server) : _stdout(dup(STDOUT_FILENO)), _server(&server) {}

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
	_server = copy._server;
	return *this;
}

CGI::~CGI()
{
	// Je recupere tout
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

void CGI::_executeCommand(const std::string& cmd, const std::string& query, char **args, char **envp, int *pipefd)
{
	Logger::log("Running command : " + cmd, DEBUG);
	// Processus enfant
	dup2(pipefd[1], STDOUT_FILENO); // Rediriger la sortie standard vers le pipe
	close(pipefd[0]); // Fermer l'extrémité en lecture du pipe
	close(pipefd[1]); // Fermer l'extrémité en écriture après duplication

	// Utiliser un pipe pour envoyer les données POST via l'entrée standard
	int *pipe_stdin = createPipes();

	dup2(pipe_stdin[0], STDIN_FILENO);  // Rediriger stdin vers le pipe
	close(pipe_stdin[0]);
	if (query.size() > 0)
		write(pipe_stdin[1], query.c_str(), query.size()); // Envoyer les données POST
	close(pipe_stdin[1]);
	Logger::log("query : " + query, DEBUG);
	execve(cmd.c_str(), args, envp); // Exécuter PHP CGI
	_restoreStdOut();
	throw std::runtime_error("Unkown command : " + cmd);
}

std::string extractAfterPrefix(const std::string& input, const std::string& prefix)
{
	size_t pos = input.find(prefix);  // Trouver la position du prefixe
		
	if (pos == std::string::npos)
		return "";

	// Deplacer juste apres le prefixe
	pos += prefix.length();

	// Trouver le premier '\n'
	size_t endPos = input.find("\n", pos);

	if (endPos == std::string::npos)
		endPos = input.length();  // Si pas de '\n'

	// Extraire la sous chaine
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
		std::string line = input.substr(start, pos - start);  // Extraire la ligne
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
	rep.attributes["Location"] = Utils::removeWSpaces(extractAfterPrefix(output, "Location: "));
	rep.attributes["Content-Type"] = Utils::removeWSpaces(extractAfterPrefix(output, "Content-type: "));
	rep.status = std::atoi(Utils::removeWSpaces(extractAfterPrefix(output, "Status: ")).c_str());
	rep.phrase = "OK";
	rep.body = removeHeader(output);
	rep.ready = true;

	_handlePHPSession(req, rep);
	return rep;
}

Response handleParentProcessPHP(int* pipefd, pid_t pid, CGI* cgi, Request& req)
{
	close(pipefd[1]);
	std::string result = Utils::readFD(pipefd[0]);
	close(pipefd[0]);

	//Logger::log(result, TEXT);
	
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
			Logger::log("Child process code : " + Utils::toString(WEXITSTATUS(status)), DEBUG);
	return cgi->_parseOutputPHP(result, req);
}

std::string CGI::_getQuery(const Request& req)
{
	if (req.method == "POST") {
		// Vérifier si Content-Type existe
		std::map<std::string, std::string>::const_iterator it = req.attributes.find("Content-Type");
		if (it == req.attributes.end()) {
			Logger::log("POST request missing Content-Type", ERROR);
			throw std::runtime_error("Missing Content-Type in POST request");
		}

		// Traitement spécial pour multipart/form-data
		if (it->second.find("multipart/form-data") != std::string::npos) {
			try {
				std::string contentType = it->second;
				size_t boundaryPos = contentType.find("boundary=");
				if (boundaryPos == std::string::npos) {
					throw std::runtime_error("Missing boundary in multipart/form-data");
				}
				std::string boundary = contentType.substr(boundaryPos + 9);
				
				// Vérifier la validité du boundary
				if (boundary.empty()) {
					throw std::runtime_error("Empty boundary in multipart/form-data");
				}

				// Extraire le nom du fichier avec vérification
				std::string filename = "unknown";
				size_t filenamePos = req.request.find("filename=\"");
				if (filenamePos != std::string::npos) {
					filenamePos += 10;
					size_t filenameEnd = req.request.find("\"", filenamePos);
					if (filenameEnd == std::string::npos) {
						throw std::runtime_error("Malformed filename in multipart/form-data");
					}
					filename = req.request.substr(filenamePos, filenameEnd - filenamePos);
				}

				// Vérifier la taille du body
				if (req.body.empty()) {
					throw std::runtime_error("Empty body in POST request");
				}
				
				// Construction du corps de la requête avec vérification de la mémoire
				std::string header = "--" + boundary + "\r\n";
				header += "Content-Disposition: form-data; name=\"fichier\"; filename=\"" + filename + "\"\r\n";
				header += "Content-Type: " + contentType + "\r\n\r\n";
				std::string footer = "\r\n--" + boundary + "--\r\n";
				
				size_t totalSize = header.size() + req.body.size() + footer.size();
				if (totalSize > MAX_POST_SIZE) {  // Définir MAX_POST_SIZE dans les constantes
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
				
				return std::string(body.begin(), body.end());
			}
			catch (const std::exception& e) {
				Logger::log("Error in _getQuery: " + std::string(e.what()), ERROR);
				throw; // Propager l'erreur pour une gestion appropriée
			}
		}
		return req.body;
	}
	
	if (req.method == "GET") {
		size_t pos = req.path.find("?");
		if (pos != std::string::npos) {
			return req.path.substr(pos + 1);
		}
	}
	return "";
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

Response CGI::_execPHP(const std::string& path, Request& req)
{
	std::string php_cgi = _getExec("php-cgi");
	char *args[] = {(char*)php_cgi.c_str(), (char*)path.c_str(), NULL};
	Logger::log("path : " + std::string(args[0]), DEBUG);

	// Récupérer la session depuis le serveur
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
			// Récupérer la session depuis le serveur
			if (_server && _server->hasSession(session_id))
				session = _server->getSession(session_id);
		}
	}

	// Préparer la query et calculer sa taille
	std::string query = _getQuery(req);
	
	// Variables d'environnement pour l'upload
	std::string request_method = "REQUEST_METHOD=" + req.method;
	std::string content_type = "CONTENT_TYPE=" + req.attributes["Content-Type"];
	// Utiliser la taille de la query au lieu de req.body.size()
	std::string content_length = "CONTENT_LENGTH=" + Utils::toString(query.length());
	std::string script_filename = "SCRIPT_FILENAME=" + path;
	std::string session_env = "HTTP_COOKIE=PHPSESSID=" + session_id;
	std::string upload_tmp_dir = "UPLOAD_TMP_DIR=/tmp";
	std::string gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
	std::string server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
	std::string document_root = "DOCUMENT_ROOT=" + path.substr(0, path.find_last_of("/"));
	std::string script_name = "SCRIPT_NAME=" + path.substr(path.find_last_of("/"));
	std::string query_string = "QUERY_STRING=" + query;

	// Log des informations importantes
	Logger::log("Content-Length original: " + Utils::toString(req.body.size()), DEBUG);
	Logger::log("Content-Length final: " + Utils::toString(query.length()), DEBUG);
	Logger::log("Upload tmp dir: " + upload_tmp_dir, DEBUG);

	// Préparer les variables de session pour PHP
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

	// Créer les pipes
	int* pipefd = createPipes();
	if (pipefd == NULL)
		throw std::runtime_error("Unable to create pipe");

	// Créer un nouveau processus avec fork
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
			Logger::log(e.what(), ERROR);
			exit(EXIT_FAILURE);
		}
	}
	else
		return handleParentProcessPHP(pipefd, pid, this, req);
	Response rep;
	return rep;
}

Response CGI::_execPython(const std::string& path, Request& req)
{
	// Nettoyer le chemin du script en retirant les paramètres GET
	std::string clean_path = path;
	size_t pos = clean_path.find("?");
	if (pos != std::string::npos) {
		clean_path = clean_path.substr(0, pos);
	}

	std::string python_exec = _getExec("python3");
	char *args[] = {(char*)python_exec.c_str(), (char*)clean_path.c_str(), NULL};
	
	std::string query = _getQuery(req);
	std::string request_method = "REQUEST_METHOD=" + req.method;
	std::string content_type = "CONTENT_TYPE=" + req.attributes["Content-Type"];
	std::string content_length = "CONTENT_LENGTH=" + Utils::toString(query.length());
	std::string script_filename = "SCRIPT_FILENAME=" + clean_path;
	std::string gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
	std::string server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
	std::string query_string = "QUERY_STRING=" + query;

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
		try
		{
			_executeCommand(python_exec, query, args, envp, pipefd);
		}
		catch(const std::runtime_error& e)
		{
			Logger::log(e.what(), ERROR);
			exit(EXIT_FAILURE);
		}
	}
	else
		return handleParentProcessPHP(pipefd, pid, this, req);
	Response rep;
	return rep;
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
	// Nettoyer le chemin du fichier en retirant les paramètres GET
	std::string clean_path = path;
	size_t pos = clean_path.find("?");
	if (pos != std::string::npos) {
		clean_path = clean_path.substr(0, pos);
	}
	
	std::string query = _getQuery(req);
	
	// Compiler le programme C
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
			
			// Préparer les variables d'environnement pour le programme C
			std::string query_string = "QUERY_STRING=" + query;
			char* envp[] = {
				(char*)query_string.c_str(),
				NULL
			};
			
			_executeCommand(executable, query, args, envp, pipefd);
		}
		catch(const std::runtime_error& e)
		{
			Logger::log(e.what(), ERROR);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		Response rep = handleParentProcessPHP(pipefd, pid, this, req);
		remove(executable.c_str()); // Supprimer l'exécutable après utilisation
		return rep;
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
		ext = clean_path.substr(dot_pos);  // Inclut le point
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
		rep = _execPHP(path, req);
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

	// Ajouter le cookie de session à la réponse si nécessaire
	if (session_id.empty())
	{
		session_id = Utils::generateRandomString(32);
		rep.attributes["Set-Cookie"] = "PHPSESSID=" + session_id + "; Path=/";
	}
}