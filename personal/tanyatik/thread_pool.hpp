#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <queue>
#include <list>

#include "lock_free_queue.hpp"

namespace tanyatik {

class ThreadJoiner {
private:
    std::vector<std::thread> &threads_;
public:
    explicit ThreadJoiner(std::vector<std::thread> &threads)
        : threads_(threads)
        {}

    ~ThreadJoiner() {
        for (auto &thread: threads_) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    }
};

class ThreadPool {
private: 
    std::atomic_bool done_;
    LockFreeQueue<std::function<void()>> task_queue_;
    std::vector<std::thread> workers_;
    ThreadJoiner joiner_;

    void doWork() {
        while (!done_) {
            std::function<void()> task;

            if (task_queue_.tryPop(&task)) {
                task();
            }
            else {
                std::this_thread::yield();
            }
        }
    }

public:
    ThreadPool(size_t workers_count = std::thread::hardware_concurrency()) :
        done_(false),
        joiner_(workers_) {
        
        try {
            for (size_t worker_number = 0; worker_number < workers_count; ++worker_number) {
                workers_.emplace_back(std::thread(&ThreadPool::doWork, this));
            }
        } catch(...) {
            done_ = true;   
            throw;
        }
    }

    ~ThreadPool() {
        done_ = true;
    }
    
    template<typename FunctionType>
    void submit(FunctionType function) {
        task_queue_.push(std::function<void()>(function));
    }
};

} // namespace tanyatik
