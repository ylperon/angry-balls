#pragma once

#include "mio.hpp"

namespace mio {

class ServerConnection : public mio::Connection,
    public std::enable_shared_from_this<ServerConnection> {
private:
    ServerConnection(std::shared_ptr<mio::ConnectionManager> connection_manager,
            std::shared_ptr<mio::ServerSocket> server_socket, 
            std::shared_ptr<mio::Reader> reader) :

        Connection(server_socket, reader, nullptr, nullptr) {
        std::shared_ptr<ServerConnection> this_ptr(this); 
        connection_manager->addConnection(this_ptr);
    }

public:
    static std::shared_ptr<ServerConnection> create
        (std::weak_ptr<mio::ConnectionManager> connection_manager,
         std::shared_ptr<mio::ServerSocket> server_socket) {

        auto reader = std::make_shared<ServerAcceptor>
            (server_socket, connection_manager); 
        std::shared_ptr<mio::ConnectionManager> conn_m = connection_manager.lock();
        if (conn_m) {
            return (new ServerConnection(conn_m, server_socket, reader))->shared_from_this();
        } else {
            return nullptr;
        }
    }

    virtual void addOutput(mio::Buffer output) {}
};

} // namespace mio
