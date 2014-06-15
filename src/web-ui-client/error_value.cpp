#include "webserver.h"

ErrorValue ErrorValue::ErrorFromErrno(const std::string& message)
{
    return ErrorValue(message + ". " + std::string(strerror(errno)));
}
