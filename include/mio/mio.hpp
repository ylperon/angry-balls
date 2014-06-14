#pragma once

#include <memory>
#include <utility>
#include <list>
#include <queue>
#include <stdexcept>

#include "socket.hpp"

namespace mio {

using BufferVector = std::vector<char>;
using Buffer = std::shared_ptr<BufferVector>;

template<typename... Args>
Buffer createBuffer(Args... args) {
    return std::make_shared<BufferVector>(args...);
}

class Socket;

class Reader {
public:
    virtual bool read() = 0;
    virtual ~Reader() {}
};

class Writer {
public:
    virtual void write(Buffer /*buffer*/) {}
    virtual ~Writer() {}
};

class Closer {
public:
    virtual void onClose() {};
    virtual ~Closer() {}
};

class Connection;

class ConnectionManager {
public:
    virtual std::shared_ptr<Connection> addConnection(std::shared_ptr<Connection>) = 0;
};

class ConnectionFactory {
private:
    std::weak_ptr<ConnectionManager> connection_manager_;
    virtual std::shared_ptr<Connection> createConnectionImpl(std::shared_ptr<Socket>) = 0;

public:
    ConnectionFactory(std::weak_ptr<ConnectionManager> connection_manager) :
        connection_manager_(connection_manager)
        {}

    std::shared_ptr<Connection> createConnection(std::shared_ptr<Socket> socket) {
        auto connection = createConnectionImpl(socket);

        auto conn_m = connection_manager_.lock();
        if (conn_m) {
            conn_m->addConnection(connection);
        }

        return connection;
    }
};

class InputProtocol {
public:
    virtual ~InputProtocol() {}
    virtual void processDataChunk(Buffer buffer) = 0;
};

class OutputProtocol {
public:
    virtual Buffer getResponse(Buffer) = 0;
    virtual ~OutputProtocol() {}
};

class RequestHandler {
public:
    virtual void handleRequest(Buffer request) = 0;
    virtual ~RequestHandler() {}
};

} // namespace mio
