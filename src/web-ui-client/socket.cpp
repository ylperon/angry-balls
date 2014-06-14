#include "webserver.hpp"

#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

Socket::Socket()
    : has_fd(false)
    , fd(0)
{
}

Socket::Socket(int fd)
    : has_fd(true)
    , fd(fd)
{
}

int Socket::GetFd() const
{
    return fd;
}

void Socket::Set(int fd)
{
    if (has_fd)
        Close();

    has_fd = true;
    this->fd = fd;
}

void Socket::Close()
{
    if (!has_fd)
        throw std::logic_error("Called Socket::Close() on Socket that does not own fd");

    // std::cerr << "closing socket " << fd << std::endl;
    if (close(fd) < 0 && errno != EINTR)
        std::cerr << "WARNING: Failed to close socket: "
                  << std::string(strerror(errno))
                  << std::endl;

    has_fd = false;
    fd = 0;
}

int Socket::Disown()
{
    if (!has_fd)
        throw std::logic_error("Called Socket::Disown() on Socket that does not own fd");

    int result = fd;
    fd = 0;
    has_fd = false;
    return result;
}

Socket::~Socket()
{
    if (has_fd)
        Close();
}
