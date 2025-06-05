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
#include <cstdlib>
#include <cstring>
#include <stdexcept>

std::string CgiHandler::execute()
{
    run();
    if (output.empty())
        throw std::runtime_error("CGI script returned no output");

    return (getOutput());
}

CgiHandler::CgiHandler(std::string &scriptPath, const Request &req, std::string &method)
    : _method(method), _body(req.getBody()), _headers(req.getHeaders()), path(scriptPath)
{
    pid = 0;
    memset(&pipefdIn, 0, sizeof(pipefdIn));
    memset(&pipefdOut, 0, sizeof(pipefdOut));
    try
    {
        isFile();
        isExecutable();
        createPipe();
        createChild();
    }
    catch (const std::exception &e)
    {
        cleanup();
        throw;
    }
}

CgiHandler::~CgiHandler()
{
}

void CgiHandler::cleanup()
{
    int status = 0;

    if (pipefdOut[0])
        close(pipefdOut[0]);
    if (pipefdOut[1])
        close(pipefdOut[1]);
    if (pipefdIn[1])
        close(pipefdIn[1]);
    if (pipefdIn[0])
        close(pipefdIn[0]);
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
    if (pipe(pipefdIn) == -1 || pipe(pipefdOut) == -1)
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
    try
    {
        runChild();
        runParent();
    }
    catch (std::exception &e)
    {
        cleanup();
        throw;
    }
}

void CgiHandler::runChild()
{
    if (pid == 0)
    {
        if (dup2(pipefdOut[1], STDOUT_FILENO) == -1)
            exit(1);

        if (_method == "POST")
        {
            if (dup2(pipefdIn[0], STDIN_FILENO) == -1)
                exit(1);
        }
        if (close(pipefdIn[0]) == -1 || close(pipefdOut[1]) == -1)
            exit(1);
        std::string contentLength = std::to_string(_body.size());
        std::string contentType = _headers.count("Content-Type") ? _headers.at("Content-Type") : "application/x-www-form-urlencoded";


        std::string request_method = "REQUEST_METHOD=" + _method;
        std::string content_length = "CONTENT_LENGTH=" + contentLength;
        std::string content_type = "CONTENT_TYPE=" + contentType;

        char *envp[] = {
            (char *)"GATEWAY_INTERFACE=CGI/1.1",
            (char *)"SERVER_PROTOCOL=HTTP/1.1",
            (char *)request_method.c_str(),
            (char *)content_length.c_str(),
            (char *)content_type.c_str(),
            NULL};
        char *argv[] = {(char *)path.c_str(), NULL};

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
        int status;
        int got_pid = 0;
        struct pollfd poll_fd = {
            .fd = pipefdOut[0],
            .events = POLL_IN,
            .revents = 0,
        };
        uint32_t wait_ms = 3000;
        int poll_ret;
        if (_method == "POST" && !_body.empty())
        {
            ssize_t written = write(pipefdIn[1], _body.c_str(), _body.size());
            if (written < 0)
            {
                throw std::runtime_error("Failed to write POST body to CGI script");
            }
        }

        if (close(pipefdOut[1]) == -1)
            throw std::runtime_error("failure closing pipe");
        poll_ret = poll(&poll_fd, 1, (int)wait_ms);
        if (poll_ret == 0)
        {
            throw timeout_exception("cgi timed out");
            kill(pid, SIGKILL);
        }
        else if (poll_ret == -1)
            throw std::runtime_error("poll error");
        while ((n = read(pipefdOut[0], buffer, sizeof(buffer))) > 0)
            output.append(buffer, n);
        if (n == -1)
            throw std::runtime_error("reading error");
        if (close(pipefdOut[0]) == -1)
            throw std::runtime_error("failure closing pipe");
        while (got_pid == waitpid(pid, &status, 0))
        {
            if ((got_pid != -1) || (errno != EINTR))
                break;
        }
        if (got_pid == -1)
            throw std::runtime_error("waitpid error");
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) == 1)
                throw std::runtime_error("execve failed");
        }
    }
}
