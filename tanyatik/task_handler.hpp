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

} // namespace tanyatik
