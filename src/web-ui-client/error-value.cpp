#include "webserver.hpp"

ErrorValue ErrorValue::error_from_errno(const std::string& message)
{
    return ErrorValue(message + std::string(strerror(errno)));
}
