#pragma once

#include "mio/mio.hpp"
#include "mio/server_socket.hpp"

namespace mio {

class ServerAcceptor : public mio::Reader {
private:
    std::shared_ptr<mio::ServerSocket> socket_;
    std::shared_ptr<mio::ConnectionFactory> connection_factory_;

public:
    ServerAcceptor(std::shared_ptr<mio::ServerSocket> socket,
            std::shared_ptr<mio::ConnectionFactory> connection_factory) :
        socket_(socket),
        connection_factory_(connection_factory)
        {}

    bool read() {
        while (true) {
            auto new_socket = socket_->acceptNewConnection();
            if (new_socket != nullptr) {
                connection_factory_->createConnection(new_socket);
            } else {
                break;
            }
        }
        return false;
    }
};

} // namespace mio
