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


std::string CgiHandler::execute(std::string &scriptPath)
{
    CgiHandler cgiHandler(scriptPath);
    cgiHandler.run();
    return (cgiHandler.getOutput());
}

CgiHandler::CgiHandler(std::string &scriptPath)
{
    path = scriptPath;
    try {
        isFile();
        isExecutable();
        createPipe();
        createChild();
    } catch (const std::exception& e) {
        cleanup();
        throw;
    }
}

CgiHandler::~CgiHandler()
{
}

void CgiHandler::cleanup()
{
    int status;

    close(pipefd[0]);
    close(pipefd[1]);
    if (pid != 0)
    {
        if (kill(pid, 0) == 0)
            kill(pid, SIGKILL);
    }

    waitpid(pid, &status, 0);
}

std::string CgiHandler::getOutput() const
{
    return (output);
}

void CgiHandler::isFile()
{
    if (access(path.c_str(), F_OK) == -1)
        throw std::invalid_argument("file does not exist");
}

void CgiHandler::isExecutable()
{
    if (access(path.c_str(), X_OK) == -1)
        throw std::logic_error("file is not executable");
}

void CgiHandler::createPipe(void)
{
    if (pipe(pipefd) == -1)
        throw std::runtime_error("failure creating pipe");
}

void CgiHandler::createChild(void)
{
    pid = fork();
    if (pid == -1)
        throw std::runtime_error("failure creating child");
}

void CgiHandler::run()
{
    try {
        runChild();
        runParent();
    } catch (std::exception& e) {
        cleanup();
        throw;
    }
}

void CgiHandler::runChild()
{
    if (pid == 0)
    {
        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
            exit(1);
        if (close(pipefd[0]) == -1 || close(pipefd[1] == -1))
            exit(1);

        char* envp[] = {
            (char*)"GATEWAY_INTERFACE=CGI/1.1",
            (char*)"SERVER_PROTOCOL=HTTP/1.1",
            (char*)"REQUEST_METHOD=GET",
            NULL
        };
        char* argv[] = { (char*)path.c_str(), NULL };

        execve(path.c_str(), argv, envp);
        exit(1); 
    } 
}

void CgiHandler::runParent(void)
{
    if (pid != 0) 
    {
        char buffer[4096];
        ssize_t n;
        struct pollfd poll_fd = {
            .fd = pipefd[0],
            .events = POLL_IN,
            .revents = 0, 
        };
        uint32_t wait_ms = 1000;
        int poll_ret;

        if (close(pipefd[1]) == -1)
            throw std::runtime_error("failure closing pipe");
        poll_ret = poll(&poll_fd, 1, (int)wait_ms);
        if (poll_ret == 0)
        {
            throw timeout_exception("cgi timed out");
            kill(pid, SIGKILL);
        }
        else if (poll_ret == -1)
            throw std::runtime_error("poll error");
        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
            output.append(buffer, n);
        if (n == -1)
            throw std::runtime_error("reading error");
        if (close(pipefd[0]) == -1)
            throw std::runtime_error("failure closing pipe");
        waitpid(pid, NULL, 0);
    }
}


