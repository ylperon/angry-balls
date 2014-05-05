#pragma once

#include <memory>

#include <netdb.h>
#include <arpa/inet.h>

#include "io_descriptor.hpp"

namespace tanyatik {

struct InternetAddress {
private:
    std::string address_;
    int port_;

public:
    explicit InternetAddress(std::string address, int port) :
        address_(address),
        port_(port)
        {}

    struct sockaddr getSocketAddress() const {
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(port_);
        address.sin_addr.s_addr = inet_addr(address_.c_str());
        memset(address.sin_zero, '\0', sizeof address.sin_zero);
        return *((sockaddr *) &address);
    } 
    
    ~InternetAddress() {}
};

class ServerSocket : public IODescriptor {
private:
    //int fd_;
    bool non_blocking_;

    int initSocket() {
        int fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            throw std::runtime_error("Failed to create socket");
        }
        return fd;
    }

    void bindToAddress(const InternetAddress &internet_address) {
        struct sockaddr address = internet_address.getSocketAddress();
        int bound = ::bind(fd_, (struct sockaddr *) &address, sizeof(address));
        if (bound == -1) {
            throw std::runtime_error("Failed to bind socket");
        }
    }

    void listenTo() {
        int listen_result = ::listen(fd_, 10);
        if (listen_result == -1) {
            throw std::runtime_error("Failed to listen");
        } 
    }

    void closeSocket() {
        ::close(fd_);
    }

public:
    ServerSocket(const InternetAddress &internet_address, bool non_blocking = true) :
        IODescriptor(initSocket(), true),
        non_blocking_(non_blocking) {
        bindToAddress(internet_address);
        listenTo();
    }

    std::shared_ptr<IODescriptor> acceptNewConnection() {
        struct sockaddr internet_address;
        socklen_t length;
        int new_fd = ::accept(fd_, &internet_address, &length);
        if (new_fd == -1) { 
            if (non_blocking_ && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                return std::shared_ptr<IODescriptor>(nullptr);
            } else {
                std::cerr << errno << " " << (errno == EAGAIN) << " " << non_blocking_ << std::endl;
                throw std::runtime_error("Failed to accept socket");
            }
        }

        char hostname_buffer[NI_MAXHOST];
        char socket_buffer[NI_MAXSERV];
        auto result = getnameinfo (&internet_address, length,
                hostname_buffer, sizeof(hostname_buffer),
                socket_buffer, sizeof(socket_buffer),
                NI_NUMERICHOST | NI_NUMERICSERV);

        if (result == 0) {
            std::cerr << "Accepted connection on descriptor "
                << new_fd << ", host " << hostname_buffer << ", port " << socket_buffer << std::endl;
        }

        return std::shared_ptr<IODescriptor>(new IODescriptor(new_fd, non_blocking_));
    }
};

} // namespace tanyatik
