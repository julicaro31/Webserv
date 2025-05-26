/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   CgiHandler.hpp                                     :+:    :+:            */
/*                                                     +:+                    */
/*   By: nakanoun <nakanoun@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/04/21 16:36:25 by nakanoun      #+#    #+#                 */
/*   Updated: 2025/04/21 16:36:25 by nakanoun      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include "Request.hpp"
#include "Server.hpp"
#include <csignal>
#include <exception>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <filesystem>
#include <poll.h>
#include "Timeout.hpp"


class CgiHandler {
public:
    static std::string execute(std::string& scriptPath);
private:
    CgiHandler(std::string& scriptPath);
    ~CgiHandler();
    void cleanup();
    std::string getOutput() const;
    void isFile();
    void isExecutable();
    void createPipe(void);
    void createChild(void);
    void run();
    void runChild();
    void runParent();
    int pipefd[2];
    pid_t pid;
    std::string output;
    std::string path;
};

#endif
