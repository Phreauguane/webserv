/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmidou <rmidou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 15:27:16 by rmidou            #+#    #+#             */
/*   Updated: 2024/10/07 17:11:12 by rmidou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.h"

std::string intToString(int number)
{
	std::stringstream ss;
	ss << number; 
	return ss.str();
}

void executeChildProcess(const char* php_cgi, char* args[], char* envp[], int* pipefd, const std::string& query)
{
	// Processus enfant
	dup2(pipefd[1], STDOUT_FILENO); // Rediriger la sortie standard vers le pipe
	close(pipefd[0]); // Fermer l'extrémité en lecture du pipe
	close(pipefd[1]); // Fermer l'extrémité en écriture après duplication

	// Utiliser un pipe pour envoyer les données POST via l'entrée standard
	int pipe_stdin[2];
	if (pipe(pipe_stdin) == -1)
	{
		throw std::runtime_error("Failed to create stdin pipe");
		_exit(EXIT_FAILURE);
	}

	dup2(pipe_stdin[0], STDIN_FILENO);  // Rediriger stdin vers le pipe
	close(pipe_stdin[0]);
	write(pipe_stdin[1], query.c_str(), query.length()); // Envoyer les données POST
	close(pipe_stdin[1]);

	execve(php_cgi, args, envp); // Exécuter PHP CGI
	_exit(EXIT_FAILURE); // Si execve échoue
}

std::string handleParentProcess(int* pipefd, pid_t pid)
{
	close(pipefd[1]); // Fermer le côté écriture du pipe

	std::string result;
	char buffer[128];
	int bytesRead;
	while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[bytesRead] = '\0';
		result += buffer;
	}
	close(pipefd[0]);
	
	// Attendre la fin du processus enfant
	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		std::cout << "The child process ended with the code " << WEXITSTATUS(status) << std::endl;
	return result;

	// Parse output of the php process, return Response instead of string.
}


int* createPipes()
{
	static int pipefd[2];
	if (pipe(pipefd) == -1)
	{
		throw std::runtime_error("Failed to create pipe");
		return NULL;
	}
	return pipefd;
}


std::string Cgi::executePHP(const std::string& scriptPath, const std::string& query)
{
	// a aller chercher dans envp
	const char* php_cgi = "/usr/bin/php-cgi"; // Chemin vers l'interpréteur PHP

	// Arguments pour execve
	char* args[] = {(char*)php_cgi, (char*)scriptPath.c_str(), NULL};

	// Variables d'environnement
	std::string request_method = "REQUEST_METHOD=POST"; /// changer whl
	std::string content_type = "CONTENT_TYPE=application/x-www-form-urlencoded";
	std::string content_length = "CONTENT_LENGTH=" + intToString(query.length());
	std::string script_filename = "SCRIPT_FILENAME=" + scriptPath;

	char* envp[] = {
		(char*)request_method.c_str(),
		(char*)content_length.c_str(),
		(char*)content_type.c_str(),
		(char*)script_filename.c_str(),
		(char*)"REDIRECT_STATUS=200", //whl les radis
		NULL
	};

	// Créer les pipes
	int* pipefd = createPipes();
	if (pipefd == NULL)
		return "";

	// Créer un nouveau processus avec fork
	pid_t pid = fork();
	if (pid < 0)
	{
		throw std::runtime_error("Fork failed");
		return "";
	}
	else if (pid == 0)
	{
		// Processus enfant
		executeChildProcess(php_cgi, args, envp, pipefd, query);
	}
	else
	{
		// Processus parent
		return handleParentProcess(pipefd, pid);
	}
	return "";
}
