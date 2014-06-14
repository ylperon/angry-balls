#pragma once

#include "mio.hpp"
#include "server_acceptor.hpp"

namespace mio {

class ServerConnection : public mio::Connection {
public:
    ServerConnection(std::shared_ptr<mio::ServerSocket> server_socket,
            std::shared_ptr<mio::ConnectionFactory> connection_factory) :
        Connection(server_socket,
                std::make_shared<mio::ServerAcceptor>
                (server_socket, connection_factory),
                nullptr,
                nullptr) {}

    virtual void addOutput(mio::Buffer /*output*/) {}
};

} // namespace mio
