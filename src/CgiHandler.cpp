/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CgiHandler.cpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: nakanoun <nakanoun@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/04/21 16:36:28 by nakanoun      #+#    #+#                 */
/*   Updated: 2025/04/21 16:36:28 by nakanoun      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

bool CgiHandler::isFile(std::string &scriptPath)
{
    return (access(scriptPath.c_str(), F_OK) != -1);
}

bool CgiHandler::isExecutable(std::string &scriptPath)
{
    return (access(scriptPath.c_str(), X_OK) != -1);
}

std::string CgiHandler::execute(std::string &scriptPath) {
    std::string output;

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return "Status: 500 Internal Server Error\r\n\r\n<h1>500 Internal Server Error</h1>";
    }

    pid_t pid = fork();
    if (pid == -1) {
        return "Status: 500 Internal Server Error\r\n\r\n<h1>500 Internal Server Error</h1>";
    }

    if (pid == 0) {
        // Child process
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        // Basic environment
		// needs to add more env variables PATH_INFO, SCRIPT_NAME, CONTENT_TYPE maybe!
        char* envp[] = {
            (char*)"GATEWAY_INTERFACE=CGI/1.1",
            (char*)"SERVER_PROTOCOL=HTTP/1.1",
            (char*)"REQUEST_METHOD=GET",
            NULL
        };

        char* argv[] = { (char*)scriptPath.c_str(), NULL };
        execve(scriptPath.c_str(), argv, envp);

        exit(1); // if execve fails
    } else {
        // Parent process
        close(pipefd[1]);

        char buffer[4096];
        ssize_t n;
        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, n);
        }
        close(pipefd[0]);
        waitpid(pid, NULL, 0);
    }

    return output;
}
