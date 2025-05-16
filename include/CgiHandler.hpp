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

#include <string>
#include "Request.hpp"
#include "Server.hpp"

class CgiHandler {
public:
    static std::string execute(std::string& scriptPath);
private:
    CgiHandler(std::string& scriptPath);
    ~CgiHandler();
    bool isFile();
    bool isExecutable();
    bool createPipe(void);
    bool createChild(void);
    void prepareHandler();
    void runChild();
    void runParent();
    std::string getOutput() const;
    int pipefd[2];
    pid_t pid;
    std::string output;
    std::string path;
};

#endif
