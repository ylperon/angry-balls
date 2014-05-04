#pragma once

#include <map>

#include "io_handler.hpp"
#include "thread_pool.hpp"

namespace tanyatik {

template<typename RequestHandler>
class TaskHandler {
private:
    ThreadPool thread_pool_;
    //TThreadPool thread_pool_;

public:
    struct Task {
    private:
        std::shared_ptr<TaskHandler> task_handler_;
        int client_id_;
       
    public:
        Task(std::shared_ptr<TaskHandler> task_handler, int client_id) :
            task_handler_(task_handler),
            client_id_(client_id)
            {}
     
        void operator() (Buffer request) {
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
    std::function<void(Buffer)> getInputTaskHandler(int client_id) {
        std::cerr << "TH: getInputTaskHandler\n";
        return std::bind(&TaskHandler::addRequestTask, 
                this, 
                client_id, 
                std::placeholders::_1);
    }
*/
}; 

} // namespace tanyatik
