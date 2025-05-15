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
    static bool isFile(std::string& scriptPath);
    static bool isExecutable(std::string& scriptPath);
    static std::string execute(std::string& scriptPath);
};

#endif
