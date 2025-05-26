#include "Timeout.hpp"

timeout_exception::timeout_exception (const std::string& what_arg)
    : std::invalid_argument(what_arg)
{
}
