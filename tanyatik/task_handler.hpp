#pragma once

#include <map>

#include "types.hpp"
#include "io_handler.hpp"
#include "thread_pool.hpp"
#include "request_handler.hpp"

namespace tanyatik {

class ThreadPoolTaskHandler : public IOTaskHandler {
private:
    ThreadPool thread_pool_;
    std::shared_ptr<RequestHandler> request_handler_;
    std::map<int, LockFreeQueue<Buffer>> result_queues_;

    void addRequestTask(int connection_id, Buffer request) {
        thread_pool_.submit(std::bind(&RequestHandler::handleRequest, 
                    request_handler_, 
                    request, 
                    connection_id));
    }

public:
    struct ThreadPoolTaskCreator : public TaskCreator {
    private:
        ThreadPoolTaskHandler *task_handler_;
        int connection_id_;
       
    public:
        ThreadPoolTaskCreator(ThreadPoolTaskHandler *task_handler, int connection_id) :
            task_handler_(task_handler),
            connection_id_(connection_id)
            {}
     
        void addTask(Buffer request) {
            task_handler_->addRequestTask(connection_id_, request);
        }
    }; 

    ThreadPoolTaskHandler(std::shared_ptr<RequestHandler> request_handler) :
        thread_pool_(),
        request_handler_(request_handler) {
            request_handler_->setResultHandler(this);
        }

    ~ThreadPoolTaskHandler() {}

    virtual std::shared_ptr<InputHandler> createInputHandler
            (std::shared_ptr<IODescriptor> descriptor) {
        // after input is completed, we need to put a task into thread pool inside TaskHandler
        // after task is completed, TaskHandler will put result back in IOServer
        auto taskCreator = std::make_shared<ThreadPoolTaskCreator>
            (this, descriptor->getDescriptor());

        return std::make_shared<AsyncInputHandler>
                (descriptor, std::make_shared<InputHttpProtocol>(taskCreator));
    }

    virtual std::shared_ptr<OutputHandler> createOutputHandler
            (std::shared_ptr<IODescriptor> descriptor) {
        return std::make_shared<AsyncOutputHandler>
                (descriptor, std::make_shared<OutputHttpProtocol>());
    }
    
    virtual void putResult(int connection_id, Buffer result) {
        //std::cerr << "PUT RESULT [" << result.get() << "]\n";
        result_queues_[connection_id].push(result);
    }

    virtual void getResult(int connection_id, Buffer *result) {
        result_queues_[connection_id].tryPop(result);
    }

    virtual bool hasResult(int connection_id) {
        return !result_queues_[connection_id].empty();
    }
}; 

} // namespace tanyatik
