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
#include <future>

#include "io_server.hpp"
#include "io_handler.hpp"
#include "protocol.hpp"
#include "thread_pool.hpp"
#include "task_handler.hpp"

namespace tanyatik {

class ProxyRequestHandler {
public:
    static std::vector<char> processRequest(std::vector<char> request) {
        std::cerr << "PROCESS REQUEST\n";
        std::cerr << "REQUEST [" << request.data() << "]\n";
        std::string respond_string("HTTP/1.1 200 OK\n\n<html>hello<html>\n");
        std::vector<char> respond_buffer(respond_string.begin(), respond_string.end()); 

        return respond_buffer;
    }

    static std::function<std::vector<char>()> getRequestProcessor(std::vector<char> request) {
        return std::bind(processRequest, request);
    }
};

struct ProxyIOHandlerCreator {
private:
    ServerTaskHandler<ProxyRequestHandler> &task_handler_;

public:
    typedef AsyncInputHandler<InputHttpProtocol> InputHandler;
    typedef AsyncOutputHandler<OutputHttpProtocol> OutputHandler;

    ProxyIOHandlerCreator(ServerTaskHandler<ProxyRequestHandler> &task_handler) :
        task_handler_(task_handler)
        {}

    InputHandler createInputHandler(IODescriptor &descriptor) {
        return InputHandler(descriptor, 
                InputHttpProtocol(task_handler_.getInputTaskHandler
                    (descriptor.getDescriptor())));
    }

    OutputHandler createOutputHandler(IODescriptor &descriptor) {
        return OutputHandler(descriptor,
                OutputHttpProtocol(task_handler_.getOutputTaskHandler
                    (descriptor.getDescriptor())));
    }
};

class ProxyServer {
private:
    ServerTaskHandler<ProxyRequestHandler> task_handler_;
    ProxyIOHandlerCreator io_handler_creator_;
    IOServer<EpollDescriptorManager, ProxyIOHandlerCreator> io_server_;

public:
    ProxyServer() :
        task_handler_(),
        io_handler_creator_(task_handler_),
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

