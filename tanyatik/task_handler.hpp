#pragma once

#include <map>
#include <mutex>

#include "types.hpp"
#include "io_handler.hpp"
#include "thread_pool.hpp"
#include "request_handler.hpp"

namespace tanyatik {

class ThreadPoolTaskHandler : public TaskCreator {
private:
    ThreadPool thread_pool_;
    std::shared_ptr<RequestHandler> request_handler_;
    static constexpr int THREAD_COUNT = 10;

public:
    ThreadPoolTaskHandler(std::shared_ptr<RequestHandler> request_handler) :
        thread_pool_(THREAD_COUNT),
        request_handler_(request_handler)
        {}

    void addTask(int connection_id, Buffer request) {
        thread_pool_.submit(std::bind(&RequestHandler::handleRequest, 
                    request_handler_, 
                    request, 
                    connection_id));
    }
};

class ProxyRespondHandler : public RespondHandler {
private:
    std::map<int, LockFreeQueue<Buffer>> result_queues_;
    std::mutex result_queues_mutex_;

public:
    ProxyRespondHandler() {}

    virtual void putResult(int connection_id, Buffer result) {
        std::lock_guard<std::mutex> lock(result_queues_mutex_);
        result_queues_[connection_id].push(result);
    }

    virtual void getResult(int connection_id, Buffer *result) {
        std::lock_guard<std::mutex> lock(result_queues_mutex_);
        result_queues_[connection_id].tryPop(result);
    }

    virtual bool hasResult(int connection_id) {
        std::lock_guard<std::mutex> lock(result_queues_mutex_);
        return !result_queues_[connection_id].empty();
    }
}; 

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

} // namespace tanyatik
