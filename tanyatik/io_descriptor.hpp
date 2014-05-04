#pragma once

#include <netdb.h>
#include <arpa/inet.h>

namespace tanyatik {

class IODescriptor {
protected:
    int fd_;
    bool have_resources_;

private:
    void makeNonblocking() {
        auto flags = fcntl (fd_, F_GETFL, 0);
        if (flags == -1) {
            throw std::runtime_error("Error in fcntl");
        }

        flags |= O_NONBLOCK;
        auto result = fcntl (fd_, F_SETFL, flags);
        if (result == -1) {
            throw std::runtime_error("Error in fcntl");
        }
    }

public:
    IODescriptor(int connection_fd, bool non_blocking_ = false) :
        fd_(connection_fd),
        have_resources_(true) {
        if (non_blocking_) {
            this->makeNonblocking();
        }
    }
   
    IODescriptor(const IODescriptor &) = delete;
    IODescriptor &operator=(const IODescriptor &) = delete;
    IODescriptor(IODescriptor &&other) {
        fd_ = other.fd_;
        other.have_resources_ = false;
        have_resources_ = true;
    }
    
    void close() {
        ::close(fd_);
        have_resources_ = false;
    }

    ~IODescriptor() {
        if (have_resources_) {
            ::close(fd_);
        }
    }
    
    // possible rakes :(
    int getDescriptor() const {
        return fd_;
    }

    bool operator == (int fd) {
        return fd_ == fd;
    }

    bool operator < (const IODescriptor &other) {
        return fd_ < other.fd_;
    }
};

} // namespace tanyatik
