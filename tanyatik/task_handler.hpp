#pragma once

#include <map>

#include "io_handler.hpp"
#include "thread_pool.hpp"
#include "request_handler.hpp"

namespace tanyatik {

class TaskHandler {
private:
    ThreadPool thread_pool_;
    std::shared_ptr<RequestHandler> request_handler_;

public:
    typedef AsyncInputHandler<InputHttpProtocol> InputHandler;
    typedef AsyncOutputHandler<OutputHttpProtocol> OutputHandler;

    struct TaskCreator {
    private:
        TaskHandler *task_handler_;
        int connection_id_;
       
    public:
        TaskCreator(TaskHandler *task_handler, int connection_id) :
            task_handler_(task_handler),
            connection_id_(connection_id)
            {}
     
        void operator()(Buffer request) {
            task_handler_->addRequestTask(connection_id_, request);
        }
    }; 

    TaskHandler(std::shared_ptr<RequestHandler> request_handler) :
        thread_pool_(),
        request_handler_(request_handler)
        {}

    ~TaskHandler() {}

    void addRequestTask(int connection_id, Buffer request) {
        thread_pool_.submit(std::bind(&RequestHandler::handleRequest, request_handler_, request));
    }

    InputHandler createInputHandler(std::shared_ptr<IODescriptor> descriptor) {
        // after input is completed, we need to put a task into thread pool inside TaskHandler
        // after task is completed, TaskHandler will put result back in IOServer
        TaskCreator taskCreator(this, descriptor->getDescriptor());

        return InputHandler(descriptor, InputHttpProtocol(taskCreator));
    }

    OutputHandler createOutputHandler(std::shared_ptr<IODescriptor> descriptor) {
        return OutputHandler(descriptor, OutputHttpProtocol());
    }
/*
    void putResult(int connection_id, Buffer result) {
        io_server_->push(std::make_pair(connection_id, result));
    }
*/
}; 

} // namespace tanyatik
