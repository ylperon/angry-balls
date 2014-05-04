#pragma once

#include <map>
#include <future>

#include "thread_pool.hpp"

namespace tanyatik {

template<typename RequestHandler>
class ServerTaskHandler {
private:
    std::map<int, std::shared_ptr<std::future<std::vector<char>>>> responds_;
    ThreadPool thread_pool_;
    std::mutex lock_;

public:
    ServerTaskHandler() {
        std::cerr << "Create task handler\n";
    }

    ~ServerTaskHandler() {
        std::cerr << "Destroy task handler\n";
    }

    void addRequestTask(int client_id, std::vector<char> request) {
        std::cerr << "addRequestTask\n";
        auto future_result = std::make_shared<std::future<std::vector<char>>>
                (thread_pool_.submit(RequestHandler::getRequestProcessor(request)));
        std::cerr << "future result\n";
        lock_.lock();
        std::cerr << "mutex\n";
        responds_.insert(std::make_pair(client_id, future_result));
        lock_.unlock();
        std::cerr << "end request task\n";
    }

    std::vector<char> getRespond(int client_id) { 
        lock_.lock();
        auto found = responds_.find(client_id);
        if (found == responds_.end()) {
            throw std::logic_error("No future respond bound for descriptor");
        }
        // block here :(
        return found->second->get();
        lock_.unlock();
    }

    std::function<void(std::vector<char>)> getInputTaskHandler(int client_id) {
        return std::bind(&ServerTaskHandler::addRequestTask, 
                this, 
                client_id, 
                std::placeholders::_1);
    }

    std::function<std::vector<char>(void)> getOutputTaskHandler(int client_id) {
        return std::bind(&ServerTaskHandler::getRespond, this, client_id);
    }
}; 

} // namespace tanyatik
