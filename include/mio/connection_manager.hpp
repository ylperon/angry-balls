#pragma once

#include "mio/mio.hpp"

namespace mio {

class NoLockConnectionManager : public mio::ConnectionManager {
private:
    std::shared_ptr<mio::IOServer<mio::EpollDescriptorManager>> io_server_;

    //std::mutex add_mutex_;

public:
    NoLockConnectionManager(std::shared_ptr<mio::IOServer<mio::EpollDescriptorManager>> server) :
        io_server_(server)
        {}

    virtual std::shared_ptr<mio::Connection> addConnection
        (std::shared_ptr<mio::Connection> connection) {
        //std::unique_lock<std::mutex> lock(add_mutex_);
        return io_server_->addConnection(connection);
    }
};

} // namespace mio
