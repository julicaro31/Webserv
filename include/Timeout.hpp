#ifndef __TIMEOUT_H_
#define __TIMEOUT_H_

#include <exception>
#include <stdexcept>
#include <string>

class timeout_exception : public std::invalid_argument
{
public:
    explicit timeout_exception( const std::string& what_arg );
};


#endif
