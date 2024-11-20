#include "Utils.h"
#include "Cgi.h"

CGI::CGI()
{
	_stdout = dup(STDOUT_FILENO);
}

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

Response parseOutputPHP(std::string& output)
{
	// TO DO
	Response rep;

	rep.http = "HTTP/1.1";
	rep.attributes["Location"] = Utils::removeWSpaces(extractAfterPrefix(output, "Location: "));
	rep.attributes["Content-Type"] = Utils::removeWSpaces(extractAfterPrefix(output, "Content-type: "));
	rep.status =  std::atoi(Utils::removeWSpaces(extractAfterPrefix(output, "Status: ")).c_str());  // si code erreur renvoyer 500  page erreur log erreur dans le terminal  fonction error_pages
	rep.phrase = "OK";
	rep.body = removeHeader(output);
	rep.ready = true;
	return rep;
}

Response handleParentProcessPHP(int* pipefd, pid_t pid)
{
	close(pipefd[1]); // Fermer le côté écriture du pipe

	std::string result = Utils::readFD(pipefd[0]);
	close(pipefd[0]);

	Logger::log(result, TEXT);
	
	// Attendre la fin du processus enfant
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		Logger::log("Child process code : " + Utils::toString(WEXITSTATUS(status)), DEBUG);
	return parseOutputPHP(result);

	// Parse output of the php process, return Response instead of string.
}

std::string CGI::_getQuery(const Request& req)
{
	Logger::log("METHODE : " + req.method, DEBUG);
	if (req.method == "POST")
		return req.body;
	if (req.method != "GET")
		return "Wallah jsais pas";

	size_t pos = req.path.find("?");
	if (pos == std::string::npos)
		return "";
	
	return req.path.substr(pos + 1, req.path.size() - pos - 1);
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
	// a aller chercher dans envp
	std::string php_cgi = _getExec("php-cgi"); // Chemin vers l'interpréteur PHP

	// Arguments pour execve
	char *args[] = {(char*)php_cgi.c_str(), (char*)path.c_str(), NULL};

	// Variables d'environnement
	std::string request_method = "REQUEST_METHOD=" + req.method;
	std::string content_type = "CONTENT_TYPE=" + req.attributes["Content-Type"];
	std::string content_length = "CONTENT_LENGTH=" + Utils::toString(req.body.size());
	std::string script_filename = "SCRIPT_FILENAME=" + path;

	char* envp[] = {
		(char*)request_method.c_str(),
		(char*)content_length.c_str(),
		(char*)content_type.c_str(),
		(char*)script_filename.c_str(),
		(char*)"REDIRECT_STATUS=200",
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
			_executeCommand(php_cgi, _getQuery(req), args, envp, pipefd);
		}
		catch(const std::runtime_error& e)
		{
			Logger::log(e.what(), ERROR);
			exit(EXIT_FAILURE);
		}
	}
	else
		return handleParentProcessPHP(pipefd, pid);
	Response rep;
	return rep;
}

bool CGI::executeCGI(Request &req, const std::string &path, Location *loc, Response &rep)
{
	// verifier que l'extension soit autorisee
	bool allowed = false;
	std::string ext = Utils::getExt(req.path);
	if (ext == "")
		return false;
	for (size_t i = 0; i < loc->_cgi_exts.size(); ++i)
	{
		if (ext == loc->_cgi_exts[i])
		{
			allowed = true;
			break;
		}
	}
	Logger::log("Not allowed :" + ext, DEBUG);
	if (!allowed)
		return false;
	if (ext == ".php")
	{
		Logger::log("Running php...", DEBUG);
		rep = _execPHP(path, req);
	}
	else
	{
		Logger::log("Unkown ext : " + ext, DEBUG);
		return false;
	}
	return true;
}