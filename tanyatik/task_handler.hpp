#pragma once

#include <map>

#include "io_handler.hpp"
#include "thread_pool.hpp"

namespace tanyatik {

template<typename RequestHandler>
class TaskHandler {
private:
    ThreadPool thread_pool_;

public:
    struct TaskCreator {
    private:
        std::shared_ptr<TaskHandler> task_handler_;
        int client_id_;
       
    public:
        TaskCreator(std::shared_ptr<TaskHandler> task_handler, int client_id) :
            task_handler_(task_handler),
            client_id_(client_id)
            {}
     
        void operator()(Buffer request) {
            task_handler_->addRequestTask(client_id_, request);
        }
    }; 

    TaskHandler() :
        thread_pool_(1) {}

    ~TaskHandler() {}

    void addRequestTask(int client_id, Buffer request) {
        thread_pool_.submit(std::bind(RequestHandler::handleRequest, request));
    }
/*
    void putResult(int client_id, Buffer result) {
        io_server_->push(std::make_pair(client_id, result));
    }
*/
}; 

} // namespace tanyatik
