#include "webserver.h"

#include <iostream>
#include <stdexcept>

#include <unistd.h>

Socket::Socket()
    : has_fd_(false)
    , fd_(0)
{
}

Socket::Socket(int fd)
    : has_fd_(true)
    , fd_(fd)
{
}

int Socket::GetFd() const
{
    return fd_;
}

void Socket::Set(int fd_other)
{
    if (has_fd_)
        Close();

    has_fd_ = true;
    fd_ = fd_other;
}

void Socket::Close()
{
    if (!has_fd_)
        throw std::logic_error("Called Socket::Close() on Socket that does not own fd");

    // std::cerr << "closing socket " << fd << std::endl;
    if (close(fd_) < 0 && errno != EINTR)
        std::cerr << "WARNING: Failed to close socket: "
                  << strerror(errno)
                  << std::endl;

    has_fd_ = false;
    fd_ = 0;
}

int Socket::Disown()
{
    if (!has_fd_)
        throw std::logic_error("Called Socket::Disown() on Socket that does not own fd");

    int result = fd_;
    fd_ = 0;
    has_fd_ = false;
    return result;
}

Socket::~Socket()
{
    if (has_fd_)
        Close();
}
