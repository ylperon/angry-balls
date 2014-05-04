#pragma once

#include <thread>
#include <vector>
#include <atomic>
#include <future>
#include <queue>

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
    LockFreeQueue<std::shared_ptr<std::packaged_task<void()>>> task_queue_;
    std::vector<std::thread> workers_;
    ThreadJoiner joiner_;

    void doWork() {
        while (!done_) {
            std::shared_ptr<std::packaged_task<void()>> task;

            if (task_queue_.dequeue(&task)) {
                (*task)();
            } else {
                std::this_thread::yield();
            }
        }
    }

public:
    template<typename ResultType>
    using TaskHandle = std::future<ResultType>;

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
    TaskHandle<typename std::result_of<FunctionType()>::type> submit(FunctionType function) {
        typedef typename std::result_of<FunctionType()>::type ResultType;

        auto task = std::make_shared<std::packaged_task<ResultType()>>(function);

        TaskHandle<ResultType> future_result(task->get_future());

        task_queue_.push(std::make_shared<std::packaged_task<void()>>([=]() {(*task)();}));

        return future_result;
    }
};

} // namespace tanyatik
