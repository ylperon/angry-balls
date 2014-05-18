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

#include "io_server.hpp"
#include "io_handler.hpp"
#include "protocol.hpp"
#include "thread_pool.hpp"
#include "task_handler.hpp"
#include "respond_handler.hpp"

namespace tanyatik {

class ProxyHandlerFactory : public IOHandlerFactory {
private:
    std::shared_ptr<TaskCreator> task_handler_; 

public:
    ProxyHandlerFactory(std::shared_ptr<TaskCreator> task_handler) :
        task_handler_(task_handler)
        {} 

    virtual std::shared_ptr<InputHandler> createInputHandler
            (int descriptor) {
        return std::make_shared<AsyncInputHandler>
                (descriptor, std::make_shared<InputHttpProtocol>(task_handler_, descriptor));
    }

    virtual std::shared_ptr<OutputHandler> createOutputHandler
            (int descriptor) {
        return std::make_shared<AsyncOutputHandler>
                (descriptor, std::make_shared<OutputHttpProtocol>());
    }
};

class ProxyServer {
private:
    std::shared_ptr<RespondHandler> respond_handler_;
    std::shared_ptr<RequestHandler> request_handler_;
    std::shared_ptr<TaskCreator> task_handler_;
    IOServer<EpollDescriptorManager> io_server_;

public:
    ProxyServer() :
        respond_handler_(std::make_shared<ProxyRespondHandler>()),
        request_handler_(std::make_shared<ProxyRequestHandler>(respond_handler_)),
        task_handler_(std::make_shared<ThreadPoolTaskHandler>(request_handler_)),
        io_server_(IOServerConfig(), 
                std::make_shared<ProxyHandlerFactory>(task_handler_), 
                respond_handler_, 
                false)
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
        std::cerr << "Errno: " << errno << std::endl;
    }
    return 0;
}

