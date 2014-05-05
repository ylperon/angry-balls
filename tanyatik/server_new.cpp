#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#include <iostream>
#include <vector>
#include <map>
#include <functional>

#include "io_server.hpp"
#include "io_handler.hpp"
#include "protocol.hpp"
#include "thread_pool.hpp"
#include "task_handler.hpp"

namespace tanyatik {

class ProxyServer {
private:
    std::shared_ptr<TaskHandler> task_handler_;
    IOServer<EpollDescriptorManager, TaskHandler> io_server_;

public:
    ProxyServer() :
        task_handler_(std::shared_ptr<TaskHandler>(new TaskHandler(
                std::shared_ptr<ProxyRequestHandler>(new ProxyRequestHandler)))),
        io_server_(IOServerConfig(), task_handler_)
        {}

    void run() {
        io_server_.eventLoop();
    } 
};

} // namespace tanyatik

int main() {
    try {
        tanyatik::ProxyServer proxy_server;
        proxy_server.run();
    } catch(const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
    }
    return 0;
}

