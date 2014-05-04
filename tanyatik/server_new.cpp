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

class ProxyRequestHandler {
public:
    static Buffer handleRequest(Buffer request) {
        std::cerr << "HANDLE REQUEST\n";
        std::cerr << "REQUEST [" << request.data() << "]\n";
        std::string respond_string("HTTP/1.1 200 OK\n\n<html>hello<html>\n");
        Buffer respond_buffer(respond_string.begin(), respond_string.end()); 

        return respond_buffer;
    }
};

struct ProxyIOHandlerCreator {
private:
    std::shared_ptr<TaskHandler<ProxyRequestHandler>> task_handler_;

public:
    typedef AsyncInputHandler<InputHttpProtocol> InputHandler;
    typedef AsyncOutputHandler<OutputHttpProtocol> OutputHandler;

    ProxyIOHandlerCreator(std::shared_ptr<TaskHandler<ProxyRequestHandler>> task_handler) :
        task_handler_(task_handler)
        {}

    InputHandler createInputHandler(IODescriptor &descriptor) {
        typename TaskHandler<ProxyRequestHandler>::Task 
            inputCallback(task_handler_, descriptor.getDescriptor());

        return InputHandler(descriptor, 
                InputHttpProtocol(inputCallback));
    }

    OutputHandler createOutputHandler(IODescriptor &descriptor) {
        return OutputHandler(descriptor, OutputHttpProtocol());
    }
};

class ProxyServer {
private:
    std::shared_ptr<TaskHandler<ProxyRequestHandler>> task_handler_;
    std::shared_ptr<ProxyIOHandlerCreator> io_handler_creator_;
    IOServer<EpollDescriptorManager, ProxyIOHandlerCreator> io_server_;

public:
    ProxyServer() :
        task_handler_(new TaskHandler<ProxyRequestHandler>()),
        io_handler_creator_(new ProxyIOHandlerCreator(task_handler_)),
        io_server_(IOServerConfig(), io_handler_creator_)
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

